#pragma once

#include <string>
#include <thread>
#include <memory>
#include <sw/redis++/redis++.h>

#include "control_interface.hpp"

#define REDIS_DEFAULT "redis://127.0.0.1:6379/0"

// Redis Keys and Channels
#define CHANNEL_CONTROLS "cp_controls"
#define CHANNEL_STATS "cp_stats"
#define CHANNEL_HISTOGRAM "cp_histogram"

// Triggers
#define CONTROL_TRIGGER_RECORD "rec"
#define CONTROL_TRIGGER_STILL "stll"

// Control Keys
#define CONTROL_KEY_RECORD "is_recording"
#define CONTROL_KEY_ISO "iso"
#define CONTROL_KEY_WB "awb"
#define CONTROL_KEY_COLORGAINS "cg_rb"
#define CONTROL_KEY_SHUTTER_ANGLE "shutter_a"
#define CONTROL_KEY_SHUTTER_SPEED "shutter_s"
#define CONTROL_KEY_FRAMERATE "fps"
#define CONTROL_KEY_WIDTH "width"
#define CONTROL_KEY_HEIGHT "height"
#define CONTROL_KEY_COMPRESSION "compress"
#define LV_KEY_ZOOM "lv_zoom"

using namespace sw::redis;

class RedisControl {
public:
    RedisControl(std::shared_ptr<ControlInterface> interface, std::string redis_url = REDIS_DEFAULT);
    ~RedisControl();

    void start();
    void sync(); // Added sync method for Phase 2

private:
    std::shared_ptr<ControlInterface> interface_;
    std::string redis_url_;
    std::unique_ptr<Redis> redis_;
    std::thread thread_;
    bool abortThread_;

    // Slots for outbound signals
    void onPublishState(bool is_recording);
    void onPublishStats(const std::string& stats); // Added stats slot for Phase 3

    void worker();
    void handleMessage(const std::string& msg);
    
    // Helper to parse color gains
    void parseAndSetColorGains(const std::string& val);
};
