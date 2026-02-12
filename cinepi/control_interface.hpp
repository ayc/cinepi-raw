#pragma once

#include <string>
#include <boost/signals2.hpp>

class ControlInterface
{
public:
    // Inbound Signals (External Source -> Controller)
    // Signal signature: void(int state) - e.g., 1 for start, 0 for stop
    boost::signals2::signal<void(int)> triggerRecord;
    
    // Signal signature: void(int iso)
    boost::signals2::signal<void(int)> setIso;
    
    // Signal signature: void(int shutter_speed_us)
    boost::signals2::signal<void(int)> setShutterSpeed;

    // Signal signature: void(float fps)
    boost::signals2::signal<void(float)> setFrameRate;

    // Outbound Signals (Controller -> External Source)
    // Signal signature: void(std::string stats_json)
    boost::signals2::signal<void(const std::string&)> publishStats;

    // Signal signature: void(bool is_recording)
    boost::signals2::signal<void(bool)> publishState;

    // Signal signature: void(std::string message)
    boost::signals2::signal<void(const std::string&)> systemWarning;

    virtual ~ControlInterface() = default;
};
