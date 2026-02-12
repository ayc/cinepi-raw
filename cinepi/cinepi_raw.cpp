/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2022, Csaba Nagy.
 *
 * cinepi_raw.cpp - cinepi raw dng recording app.
 */

#include <chrono>

#include "cinepi_controller.hpp"
#include "dng_encoder.hpp"
#include "output/output.hpp"
#include "control_interface.hpp"
#include "redis_control.hpp"

using namespace std::placeholders;

// The main even loop for the application.

static void event_loop(CinePIRecorder &app, CinePIController &controller)
{
	controller.start();
	controller.sync();

	RawOptions const *options = app.GetOptions();
	std::unique_ptr<Output> output = std::unique_ptr<Output>(Output::Create(options));
	app.SetEncodeOutputReadyCallback(std::bind(&Output::OutputReady, output.get(), _1, _2, _3, _4));
	app.SetMetadataReadyCallback(std::bind(&Output::MetadataReady, output.get(), _1));

	app.OpenCamera();
	app.StartEncoder();
	app.GetOptions()->sensor = app.GetCamera()->properties().get(libcamera::properties::Model).value_or(app.CameraId());

	for (unsigned int count = 0; ; count++)
	{
		// if we change framerate or sensor mode, restart the camera. 
		if(controller.configChanged()){
			if(controller.cameraRunning){
				app.StopCamera();
				app.Teardown();
			}
			app.ConfigureVideo(CinePIRecorder::FLAG_VIDEO_RAW);
			app.StartCamera();
			controller.cameraRunning = true;

			libcamera::StreamConfiguration const &cfg = app.RawStream()->configuration();
			LOG(1, "Raw stream: " << cfg.size.width << "x" << cfg.size.height << " stride " << cfg.stride << " format "
								  << cfg.pixelFormat.toString());

			controller.process_stream_info(cfg);
		}

		CinePIRecorder::Msg msg = app.Wait();

		if (msg.type == RPiCamApp::MsgType::Quit)
			return;

		if (msg.type == RPiCamApp::MsgType::Timeout)
		{
			LOG_ERROR("ERROR: Device timeout detected, attempting a restart!!!");
			app.StopCamera();
			app.StartCamera();
			continue;
		}
		if (msg.type != CinePIRecorder::MsgType::RequestComplete)
			throw std::runtime_error("unrecognised message!");

		CompletedRequestPtr &completed_request = std::get<CompletedRequestPtr>(msg.payload);

		// parse the frame info metadata for the current frame, publish to redis stats channel
		controller.process(completed_request);

		// check for record trigger signal, open a new folder if rec_start or reset frame count if _rec_stop
		int trigger = controller.triggerRec();
		if(trigger > 0){
			// controller.folderOpen = create_clip_folder(app.GetOptions(), controller.getClipNumber());
            // SessionManager handles this now inside controller.onTriggerRecord
		} else if (trigger < 0){
			// controller.folderOpen = false;
			app.GetEncoder()->resetFrameCount();
		}
	
		// send frame to dng encoder and save to disk
		if(controller.isRecording() && controller.folderOpen){
			app.EncodeBuffer(completed_request, app.RawStream(), app.LoresStream());
		}

		// check for still trigger signal, open the stills folder and save a still frame
		int still_trigger = controller.triggerStill();
		if(still_trigger){
            // Legacy still logic... should ideally move to SessionManager too
			create_stills_folder(app.GetOptions(), controller.getStillNumber());
			app.GetEncoder()->still_capture = true;
			app.EncodeBuffer(completed_request, app.RawStream(), app.LoresStream());
		}

		// show frame on display
		app.ShowPreview(completed_request, app.VideoStream());        
	}
}

int main(int argc, char *argv[])
{
	try
	{
		CinePIRecorder app;
        
        auto controlInterface = std::make_shared<ControlInterface>();
        CinePIController controller(&app, controlInterface);

		RawOptions *options = app.GetOptions();
		if (options->Parse(argc, argv))
		{
			options->mediaDest = "/media/RAW";

			if (options->verbose >= 2)
				options->Print();

            // Initialize RedisControl with the interface and start it
            // Using default redis URL or from options if available
            std::string redis_url = options->redis.value_or(REDIS_DEFAULT);
            RedisControl redisControl(controlInterface, redis_url);
            redisControl.start();

			event_loop(app, controller);
		}
	}
	catch (std::exception const &e)
	{
		LOG_ERROR("ERROR: *** " << e.what() << " ***");
		return -1;
	}
	return 0;
}
