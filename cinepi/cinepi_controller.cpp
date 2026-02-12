#include "cinepi_controller.hpp"
#include <sstream>

using namespace std;
using namespace std::chrono;

#define THREAD_SLEEP_MS 10

#define CP_DEF_WIDTH 1920
#define CP_DEF_HEIGHT 1080
#define CP_DEF_FRAMERATE 30
#define CP_DEF_ISO 400
#define CP_DEF_SHUTTER 60
#define CP_DEF_AWB 1
#define CP_DEF_COMPRESS 1

void CinePIController::sync(){
    // Defaults for initial state if Redis is empty or not yet synced via RedisControl
    width_ = CP_DEF_WIDTH;
    height_ = CP_DEF_HEIGHT;
    framerate_ = CP_DEF_FRAMERATE;
    iso_ = CP_DEF_ISO;
    shutter_speed_ = CP_DEF_SHUTTER;
    awb_ = CP_DEF_AWB;
    compression_ = CP_DEF_COMPRESS;

    options_->compression = compression_;
    options_->width = width_;
    options_->height = height_;
    options_->framerate = framerate_;
    options_->gain = iso_;
    options_->shutter = shutter_speed_ * 1e+6;
    options_->awbEn = awb_;
    options_->awb_index = 5; // daylight
    
    options_->denoise = "off";
    options_->lores_width = 400;
    options_->lores_height = 200;
    options_->mode_string = "0:0:0:0";
}

void CinePIController::process(CompletedRequestPtr &completed_request){
    CinePIFrameInfo info(completed_request->metadata);

    // Build JSON stats string
    std::stringstream ss;
    ss << "{";
    ss << \"fps\":" << completed_request->framerate << ",";
    ss << \"color_temp\":" << info.colorTemp << ",";
    ss << \"focus\":" << info.focus << ",";
    ss << \"frame_count\":" << app_->GetEncoder()->getFrameCount() << ",";
    ss << \"buffer_size\":" << app_->GetEncoder()->bufferSize();
    ss << "}";

    interface_->publishStats(ss.str());
}

void CinePIController::mainThread(){
    // This thread is now simplified. It no longer needs to poll Redis.
    // It can be used for background tasks or monitoring if needed.
    
    LOG(1, "CINEPI_CONTROLLER THREAD STARTED");

    while (!abortThread_) {
        // Placeholder for any background logic
        this_thread::sleep_for(milliseconds(THREAD_SLEEP_MS));
    }
}