/*
 * test_control_interface.cpp
 *
 * Unit test for the abstract ControlInterface using Boost.Signals2.
 */

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "cinepi/control_interface.hpp"

// Mock implementation of a controller that listens to signals
class MockController {
public:
    void onTriggerRecord(int state) {
        last_record_state = state;
        record_triggered_count++;
    }

    void onSetIso(int iso) {
        last_iso = iso;
    }

    void onSetWb(int awb) {
        last_awb = awb;
    }

    void onSetColorGains(float r, float b) {
        last_gain_r = r;
        last_gain_b = b;
    }

    void onSetShutterAngle(float angle) {
        last_shutter_angle = angle;
    }

    void onTriggerStill() {
        still_triggered = true;
    }

    int last_record_state = 0;
    int record_triggered_count = 0;
    int last_iso = 0;
    int last_awb = 0;
    float last_gain_r = 0.0f;
    float last_gain_b = 0.0f;
    float last_shutter_angle = 0.0f;
    bool still_triggered = false;
};

// Mock implementation of a control provider (like Redis)
class MockControlProvider {
public:
    MockControlProvider(ControlInterface& interface) : interface_(interface) {}

    void simulateIncomingCommand() {
        // Provider receives command "rec_start" -> triggers signal
        interface_.triggerRecord(1);
    }

    void simulateIsoChange(int iso) {
        interface_.setIso(iso);
    }

    void simulateWbChange(int awb) {
        interface_.setWb(awb);
    }

    void simulateColorGainsChange(float r, float b) {
        interface_.setColorGains(r, b);
    }

    void simulateShutterAngleChange(float angle) {
        interface_.setShutterAngle(angle);
    }

    void simulateStillTrigger() {
        interface_.triggerStill();
    }

    // Slots for outbound signals
    void onPublishStats(const std::string& stats) {
        last_stats = stats;
    }

    std::string last_stats;

private:
    ControlInterface& interface_;
};

int main() {
    std::cout << "Running ControlInterface tests..." << std::endl;

    ControlInterface ctrlInterface;
    MockController controller;
    MockControlProvider provider(ctrlInterface);

    // 1. Connect Controller slots to Interface signals (Inbound)
    ctrlInterface.triggerRecord.connect(std::bind(&MockController::onTriggerRecord, &controller, std::placeholders::_1));
    ctrlInterface.setIso.connect(std::bind(&MockController::onSetIso, &controller, std::placeholders::_1));
    ctrlInterface.setWb.connect(std::bind(&MockController::onSetWb, &controller, std::placeholders::_1));
    ctrlInterface.setColorGains.connect(std::bind(&MockController::onSetColorGains, &controller, std::placeholders::_1, std::placeholders::_2));
    ctrlInterface.setShutterAngle.connect(std::bind(&MockController::onSetShutterAngle, &controller, std::placeholders::_1));
    ctrlInterface.triggerStill.connect(std::bind(&MockController::onTriggerStill, &controller));

    // 2. Connect Provider slots to Interface signals (Outbound)
    ctrlInterface.publishStats.connect(std::bind(&MockControlProvider::onPublishStats, &provider, std::placeholders::_1));

    // 3. Test Inbound: Provider triggers signal -> Controller receives it
    provider.simulateIncomingCommand();
    assert(controller.record_triggered_count == 1);
    assert(controller.last_record_state == 1);
    std::cout << "PASS: TriggerRecord signal received." << std::endl;

    provider.simulateIsoChange(800);
    assert(controller.last_iso == 800);
    std::cout << "PASS: SetIso signal received." << std::endl;

    provider.simulateWbChange(1);
    assert(controller.last_awb == 1);
    std::cout << "PASS: SetWb signal received." << std::endl;

    provider.simulateColorGainsChange(1.5f, 2.0f);
    assert(controller.last_gain_r == 1.5f);
    assert(controller.last_gain_b == 2.0f);
    std::cout << "PASS: SetColorGains signal received." << std::endl;

    provider.simulateShutterAngleChange(180.0f);
    assert(controller.last_shutter_angle == 180.0f);
    std::cout << "PASS: SetShutterAngle signal received." << std::endl;

    provider.simulateStillTrigger();
    assert(controller.still_triggered == true);
    std::cout << "PASS: TriggerStill signal received." << std::endl;

    // 4. Test Outbound: Controller triggers signal -> Provider receives it
    ctrlInterface.publishStats("FPS: 24.0");
    assert(provider.last_stats == "FPS: 24.0");
    std::cout << "PASS: PublishStats signal received." << std::endl;

    std::cout << "All ControlInterface tests passed." << std::endl;
    return 0;
}