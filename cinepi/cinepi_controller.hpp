#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdint.h>

#include "dng_encoder.hpp"
#include "preview/preview.hpp"
#include "core/logging.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>

#include <mutex>
#include <queue>
#include <thread>

#include "utils.hpp"

#include "cinepi_frameinfo.hpp"
#include "core/stream_info.hpp"

#include "cinepi_recorder.hpp"
#include "cinepi_state.hpp"
#include "raw_options.hpp"
#include "control_interface.hpp"
#include "session_manager.hpp"

// #include <sw/redis++/redis++.h> // Moved to RedisControl

#define REDIS_DEFAULT "redis://127.0.0.1:6379/0"

// using namespace sw::redis; // Moved to RedisControl

class CinePIController : public CinePIState
{
    public:
        CinePIController(CinePIRecorder *app, std::shared_ptr<ControlInterface> interface) 
            : CinePIState(), app_(app), options_(app->GetOptions()), interface_(interface),
            folderOpen(false), abortThread_(false), cameraInit_(true), cameraRunning(false), triggerStill_(0) 
        {
            session_ = std::make_unique<SessionManager>(options_);

            // Connect signals
            interface_->triggerRecord.connect(std::bind(&CinePIController::onTriggerRecord, this, std::placeholders::_1));
            interface_->triggerStill.connect(std::bind(&CinePIController::onTriggerStill, this));
            // Add other connections here...
        };

        ~CinePIController() {
            abortThread_ = true;
            if(main_thread_.joinable()) main_thread_.join();
        };

        void start(){
            // Redis initialization moved out
            // redis_ = new Redis(options_->redis.value_or(REDIS_DEFAULT));
            // LOG(2, redis_->ping());
            main_thread_ = std::thread(std::bind(&CinePIController::mainThread, this));
        }

        void sync();

        void process(CompletedRequestPtr &completed_request);
        void process_stream_info(libcamera::StreamConfiguration const &cfg){
            // Redis calls replaced by signal
            std::string stats = cfg.toString(); 
            // TODO: Format stats properly for signal
            // interface_->publishStats(stats); 
            
            // redis_->publish(CHANNEL_STATS, cfg.toString());
            // redis_->set(CONTROL_KEY_WIDTH, std::to_string(cfg.size.width));
            // redis_->set(CONTROL_KEY_HEIGHT, std::to_string(cfg.size.height));
        }

        bool folderOpen;
        bool cameraRunning;

        bool configChanged(){
            bool c = cameraInit_;
            cameraInit_ = false;
            return c;
        }

        // Slot for TriggerRecord signal
        void onTriggerRecord(int state){
             trigger_ = state;
             if (state > 0) {
                 if (session_->startNewClip(getClipNumber())) {
                     folderOpen = true;
                     is_recording_ = true;
                 } else {
                     interface_->systemWarning("Failed to create clip folder (Disk mounted?)");
                 }
             } else {
                 folderOpen = false;
                 is_recording_ = false;
                 // app_->GetEncoder()->resetFrameCount(); // Need to handle this, maybe via app_ pointer or signal
             }
        }

        // Slot for TriggerStill signal
        void onTriggerStill() {
            triggerStill_ = 1;
            session_->ensureStillsFolder();
            still_number_++; // Increment abstract state count
        }

        int triggerRec(){
            // Legacy polling support if needed, but we want to move to signals
            // if(!disk_mounted(const_cast<RawOptions *>(options_))){
            //    return 0;
            //}
            // ...
            return trigger_;
        }

        int triggerStill(){
            int ts_ = triggerStill_;
            triggerStill_ = 0;
            return ts_;
        }

    protected:

    private:
        void mainThread();
        // void pubThread(); // Likely no longer needed or moved

        int trigger_;
        int triggerStill_;

        bool cameraInit_;

        CinePIRecorder *app_;
        RawOptions *options_;
        
        std::shared_ptr<ControlInterface> interface_;
        std::unique_ptr<SessionManager> session_;

        // Redis *redis_; // Removed

        bool abortThread_;
        std::thread main_thread_;
};
