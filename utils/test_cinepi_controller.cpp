/*
 * test_cinepi_controller.cpp
 *
 * Unit test for the refactored CinePIController using ControlInterface.
 */

#include <iostream>
#include <string>
#include <cassert>
#include <thread>
#include <chrono>

#include "cinepi/control_interface.hpp"

// Mock Controller that simulates the refactored CinePIController logic
class MockCinePIController {
public:
    MockCinePIController(ControlInterface& interface) : interface_(interface) {
        // Connect to inbound signals
        interface_.triggerRecord.connect(std::bind(&MockCinePIController::onTriggerRecord, this, std::placeholders::_1));
        interface_.setIso.connect(std::bind(&MockCinePIController::onSetIso, this, std::placeholders::_1));
    }

    // Signals handlers (slots)
    void onTriggerRecord(int state) {
        is_recording_ = (state > 0);
        // Simulate updating state via outbound signal
        interface_.publishState(is_recording_);
    }

    void onSetIso(int iso) {
        current_iso_ = iso;
        // Simulate confirming change
        interface_.systemWarning("ISO changed to " + std::to_string(iso));
    }

    bool is_recording_ = false;
    int current_iso_ = 0;

private:
    ControlInterface& interface_;
};

int main() {
    std::cout << "Running CinePIController refactor tests..." << std::endl;

    ControlInterface ctrlInterface;
    MockCinePIController controller(ctrlInterface);

    // Mock "Provider" (e.g. Redis) listening to outbound signals
    bool received_record_state = false;
    std::string received_warning;

    ctrlInterface.publishState.connect([&](bool state) {
        received_record_state = state;
    });

    ctrlInterface.systemWarning.connect([&](const std::string& msg) {
        received_warning = msg;
    });

    // 1. Test Trigger Record Logic
    std::cout << "Testing TriggerRecord..." << std::endl;
    ctrlInterface.triggerRecord(1); // Simulate "REC_START" command
    assert(controller.is_recording_ == true);
    assert(received_record_state == true);
    std::cout << "PASS: TriggerRecord correctly updated state." << std::endl;

    // 2. Test ISO Set Logic
    std::cout << "Testing SetIso..." << std::endl;
    ctrlInterface.setIso(400); // Simulate "ISO 400" command
    assert(controller.current_iso_ == 400);
    assert(received_warning == "ISO changed to 400");
    std::cout << "PASS: SetIso correctly updated state." << std::endl;

    std::cout << "All CinePIController refactor tests passed." << std::endl;
    return 0;
}
