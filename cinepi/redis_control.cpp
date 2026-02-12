#include "redis_control.hpp"
#include "core/logging.hpp"
#include <iostream>
#include <cstring>

RedisControl::RedisControl(std::shared_ptr<ControlInterface> interface, std::string redis_url)
    : interface_(interface), redis_url_(redis_url), abortThread_(false) 
{
    interface_->publishState.connect(std::bind(&RedisControl::onPublishState, this, std::placeholders::_1));
    interface_->publishStats.connect(std::bind(&RedisControl::onPublishStats, this, std::placeholders::_1));
}

RedisControl::~RedisControl() {
    abortThread_ = true;
    if(thread_.joinable()) thread_.join();
}

void RedisControl::start() {
    try {
        redis_ = std::make_unique<Redis>(redis_url_);
        LOG(2, "Redis connected: " << redis_->ping());
        
        // Perform initial sync
        sync();

        thread_ = std::thread(&RedisControl::worker, this);
    } catch (const Error &e) {
        LOG_ERROR("Redis connection failed: " << e.what());
    }
}

void RedisControl::sync() {
    if (!redis_) return;

    try {
        auto pipe = redis_->pipeline();
        auto pipe_replies = pipe.get(CONTROL_KEY_WIDTH)
                                .get(CONTROL_KEY_HEIGHT)
                                .get(CONTROL_KEY_FRAMERATE)
                                .get(CONTROL_KEY_ISO)
                                .get(CONTROL_KEY_SHUTTER_SPEED)
                                .get(CONTROL_KEY_WB)
                                .get(CONTROL_KEY_COLORGAINS)
                                .get(CONTROL_KEY_COMPRESSION)
                                .exec();

        // Note: For now we just read them. In a real sync, we'd trigger signals.
        // The original code had default fallbacks.
        // We will implement full sync logic here.

        if (auto val = pipe_replies.get<OptionalString>(2)) { // Framerate
             interface_->setFrameRate(std::stof(*val));
        }
        if (auto val = pipe_replies.get<OptionalString>(3)) { // ISO
             interface_->setIso((int)(std::stof(*val)/100.0));
        }
        if (auto val = pipe_replies.get<OptionalString>(4)) { // Shutter Speed
             interface_->setShutterSpeed(std::stoi(*val));
        }
        if (auto val = pipe_replies.get<OptionalString>(5)) { // AWB
             interface_->setWb(std::stoi(*val));
        }
        if (auto val = pipe_replies.get<OptionalString>(6)) { // Color Gains
             parseAndSetColorGains(*val);
        }
        
    } catch (const Error &e) {
        LOG_ERROR("Redis sync failed: " << e.what());
    }
}

void RedisControl::onPublishState(bool is_recording) {
    if (redis_) {
        redis_->set(CONTROL_KEY_RECORD, std::to_string(is_recording));
    }
}

void RedisControl::onPublishStats(const std::string& stats) {
    if (redis_) {
        redis_->publish(CHANNEL_STATS, stats);
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
    if (!redis_) return;

    // Triggers
    if (msg == CONTROL_TRIGGER_STILL) {
        interface_->triggerStill();
        return;
    }

    // Keys that require reading values
    auto r = redis_->get(msg);
    if(r){
        if(msg == CONTROL_KEY_RECORD){
            int req_state = std::stoi(*r);
            interface_->triggerRecord(req_state);
        } 
        else if (msg == CONTROL_KEY_ISO){
            interface_->setIso((int)(std::stoi(*r)/100.0));
        }
        else if (msg == CONTROL_KEY_WB){
            interface_->setWb(std::stoi(*r));
        }
        else if (msg == CONTROL_KEY_COLORGAINS){
            parseAndSetColorGains(*r);
        }
        else if (msg == CONTROL_KEY_SHUTTER_ANGLE){
            interface_->setShutterAngle(std::stof(*r));
        }
        else if (msg == CONTROL_KEY_SHUTTER_SPEED){
            interface_->setShutterSpeed(std::stoi(*r));
        }
        else if (msg == CONTROL_KEY_FRAMERATE){
            interface_->setFrameRate(std::stof(*r));
        }
        // ... add other keys as needed ...
        
        redis_->bgsave();
    }
}

void RedisControl::parseAndSetColorGains(const std::string& val) {
    std::string s = val;
    float r = 0.0f, b = 0.0f;
    char *ptr = strtok(&s[0], ",");
    if(ptr != NULL){
        r = std::stof(ptr);
        ptr = strtok(NULL, ",");
        if(ptr != NULL){
            b = std::stof(ptr);
        }
    }
    interface_->setColorGains(r, b);
}