#include "redis_control.hpp"
#include "core/logging.hpp"
#include <iostream>

// Redis Keys and Channels
#define CHANNEL_CONTROLS "cp_controls"
#define CHANNEL_STATS "cp_stats"

#define CONTROL_TRIGGER_RECORD "rec"
#define CONTROL_KEY_RECORD "is_recording"
#define CONTROL_KEY_ISO "iso"

RedisControl::RedisControl(std::shared_ptr<ControlInterface> interface, std::string redis_url)
    : interface_(interface), redis_url_(redis_url), abortThread_(false) 
{
    interface_->publishState.connect(std::bind(&RedisControl::onPublishState, this, std::placeholders::_1));
}

RedisControl::~RedisControl() {
    abortThread_ = true;
    if(thread_.joinable()) thread_.join();
}

void RedisControl::start() {
    try {
        redis_ = std::make_unique<Redis>(redis_url_);
        LOG(2, "Redis connected: " << redis_->ping());
        thread_ = std::thread(&RedisControl::worker, this);
    } catch (const Error &e) {
        LOG_ERROR("Redis connection failed: " << e.what());
    }
}

void RedisControl::onPublishState(bool is_recording) {
    if (redis_) {
        redis_->set(CONTROL_KEY_RECORD, std::to_string(is_recording));
    }
}

void RedisControl::worker() {
    auto sub = redis_->subscriber();
    sub.on_message([this](std::string channel, std::string msg) {
        handleMessage(msg);
    });

    sub.subscribe(CHANNEL_CONTROLS);

    while (!abortThread_) {
        try {
            sub.consume();
        } catch (const Error &err) {
            // Handle exceptions.
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void RedisControl::handleMessage(const std::string& msg) {
    auto r = redis_->get(msg);
    if(r){
        if(msg == CONTROL_KEY_RECORD){
            // In the new model, we simply pass the intent to the controller
            // The logic "trigger_ = !is_recording_" is now the controller's job
            // But here we receive "is_recording" state change request
            int req_state = std::stoi(*r);
            interface_->triggerRecord(req_state);
        } 
        else if (msg == CONTROL_KEY_ISO){
            int iso_val = (int)(std::stoi(*r)/100.0); // logic from original
            interface_->setIso(iso_val);
        }
        // ... port other commands ...
    }
}
