#pragma once

#include <string>
#include <thread>
#include <memory>
#include <sw/redis++/redis++.h>

#include "control_interface.hpp"

#define REDIS_DEFAULT "redis://127.0.0.1:6379/0"

using namespace sw::redis;

class RedisControl {
public:
    RedisControl(std::shared_ptr<ControlInterface> interface, std::string redis_url = REDIS_DEFAULT);
    ~RedisControl();

    void start();

private:
    std::shared_ptr<ControlInterface> interface_;
    std::string redis_url_;
    std::unique_ptr<Redis> redis_;
    std::thread thread_;
    bool abortThread_;

    // Slots for outbound signals
    void onPublishState(bool is_recording);

    void worker();
    void handleMessage(const std::string& msg);
};