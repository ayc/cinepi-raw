/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2020, Raspberry Pi (Trading) Ltd.
 *
 * mjpeg_encoder.cpp - mjpeg video encoder.
 */

#include <chrono>
#include <iostream>
#include <libcamera/control_ids.h>
#include <libcamera/formats.h>

#include "core/still_options.hpp"
#include "core/stream_info.hpp"

#include "dng_encoder.hpp"
#include "utils.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace libcamera;

DngEncoder::DngEncoder(RawOptions const *options)
	: Encoder(options), abortEncode_(false), abortOutput_(false), index_(0), frameStop_(0), frames_(0), resetCount_(false), encodeCheck_(false), cache_buffer_(448), compressed(false)
{
    options_ = options;
    dng_writer_ = std::make_unique<DngWriter>(options);

	for (int i = 0; i < NUM_ENC_THREADS; i++){
		encode_thread_[i] = std::thread(std::bind(&DngEncoder::encodeThread, this, i));
		cache_thread_[i] = std::thread(std::bind(&DngEncoder::cacheThread, this, i));
	}
	LOG(2, "Opened DngEncoder");
}

DngEncoder::~DngEncoder()
{
	abortEncode_ = true;
	for (int i = 0; i < NUM_ENC_THREADS; i++){
		encode_thread_[i].join();
		cache_thread_[i].join();
	}
	abortOutput_ = true;
	LOG(2, "DngEncoder closed");
}

void DngEncoder::EncodeBuffer(int fd, size_t size, void *mem, StreamInfo const &info, int64_t timestamp_us)
{
	{
		std::lock_guard<std::mutex> lock(encode_mutex_);
	}
    {
        input_done_callback_(nullptr);
        output_ready_callback_(mem, size, timestamp_us, true);
	}		
}

void DngEncoder::EncodeBuffer2(int fd, size_t size, void *mem, StreamInfo const &info, size_t losize, void *lomem, StreamInfo const &loinfo, int64_t timestamp_us, CompletedRequest::ControlList const &metadata)
{
	{
		std::lock_guard<std::mutex> lock(encode_mutex_);
		EncodeItem item = { mem, size, info, lomem, losize, loinfo, metadata, timestamp_us, index_++ };
		encode_queue_.push(item);
		encode_cond_var_.notify_all();
	}
}

void DngEncoder::encodeThread(int num)
{
	std::chrono::duration<double> encode_time(0);
	EncodeItem encode_item;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(encode_mutex_);
			while (true)
			{	

				using namespace std::chrono_literals;
				if (abortEncode_ && encode_queue_.empty())
				{
					if (frames_)
						LOG(2, "Encode " << frames_ << " frames, average time " << encode_time.count() * 1000 / frames_
										 << "ms");
					return;
				}
				if (!encode_queue_.empty())
				{
					encode_item = encode_queue_.front();
					encode_queue_.pop();
					break;
				}
				else{
					encode_cond_var_.wait_for(lock, 200ms);
				}
			}
		}

		frames_ = {encode_item.index};
		LOG(1, "memcpy frame: " << encode_item.index);

		{	
			uint8_t *mem = (uint8_t*)malloc(encode_item.size);
			memcpy(mem, encode_item.mem, encode_item.size);
			uint8_t *lomem = (uint8_t*)malloc(encode_item.losize);
			memcpy(lomem, encode_item.lomem, encode_item.losize);
			CachedItem item = { mem, encode_item.size, encode_item.info, lomem, encode_item.losize, encode_item.loinfo, encode_item.met, encode_item.timestamp_us, encode_item.index };
			std::lock_guard<std::mutex> lock(cache_mutex_);
			cache_buffer_.push_back(std::move(item));
			cache_cond_var_.notify_all();
		}

		{
			input_done_callback_(nullptr);
			output_ready_callback_(encode_item.mem, encode_item.size, encode_item.timestamp_us, true);
		}		

	}
}

void DngEncoder::cacheThread(int num)
{
	std::chrono::duration<double> cache_time(0);
	CachedItem cache_item;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(cache_mutex_);
			while (true)
			{
				using namespace std::chrono_literals;
				if (abortEncode_ && cache_buffer_.empty())
				{
					if (frames_)
						LOG(2, "Encode " << frames_ << " frames, average time " << cache_time.count() * 1000 / frames_
										 << "ms");
					return;
				}
				if (!cache_buffer_.empty())
				{
					cache_item = cache_buffer_.front();
					cache_buffer_.pop_front();
					break;
				}
				else{
					cache_cond_var_.wait_for(lock, 200ms);
				}
			}
		}

		char ft[128];
		if(still_capture){
			snprintf(ft, sizeof(ft), "%s/%s/%s_%09ld.dng", options_->mediaDest.c_str(), std::string("stills").c_str(), options_->folder.c_str(), cache_item.index);
		} else {
			snprintf(ft, sizeof(ft), "%s/%s/%s_%09ld.dng", options_->mediaDest.c_str(), options_->folder.c_str(), options_->folder.c_str(), cache_item.index);
		}
		
		std::string filename = std::string(ft);
		LOG(1, "save frame: " << cache_item.index);

		bool dm = disk_mounted(options_);
		auto start_time = std::chrono::high_resolution_clock::now();
		if(dm){
            // Using the new DngWriter
			dng_writer_->writeFrame((const uint8_t*)cache_item.mem, cache_item.info, (const uint8_t*)cache_item.lomem, cache_item.loinfo, cache_item.losize, cache_item.met, filename, cache_item.index);
		}
		auto end_time = (std::chrono::high_resolution_clock::now() - start_time);

		still_capture = false;
		
		cache_time += (end_time);
		free(cache_item.mem);
		free(cache_item.lomem);
	}
}