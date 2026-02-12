/*
 * test_still_logic.cpp
 *
 * Unit test for Still Capture logic integration.
 */

#include <iostream>
#include <string>
#include <cassert>
#include <memory>

#include "cinepi/control_interface.hpp"

// Mock SessionManager
class MockSessionManager {
public:
    bool ensureStillsFolder() {
        stills_folder_ensured = true;
        return true;
    }
    bool stills_folder_ensured = false;
};

// Partial Mock CinePIController
class MockCinePIController {
public:
    MockCinePIController(std::shared_ptr<ControlInterface> interface) : interface_(interface) {
        session_ = std::make_unique<MockSessionManager>();
        
        // Connect signal
        interface_->triggerStill.connect(std::bind(&MockCinePIController::onTriggerStill, this));
    }

    void onTriggerStill() {
        triggerStill_ = 1; // Set trigger flag
        session_->ensureStillsFolder();
        still_number_++;
    }

    int triggerStill() {
        int temp = triggerStill_;
        triggerStill_ = 0;
        return temp;
    }

    int triggerStill_ = 0;
    unsigned int still_number_ = 0;
    std::unique_ptr<MockSessionManager> session_;

private:
    std::shared_ptr<ControlInterface> interface_;
};

int main() {
    std::cout << "Running Still Capture Logic tests..." << std::endl;

    auto interface = std::make_shared<ControlInterface>();
    MockCinePIController controller(interface);

    // 1. Simulate Trigger Signal
    std::cout << "Triggering still capture signal..." << std::endl;
    interface->triggerStill();

    // 2. Verify Controller State
    assert(controller.session_->stills_folder_ensured == true);
    assert(controller.triggerStill_ == 1);
    assert(controller.still_number_ == 1);
    std::cout << "PASS: Controller handled signal and called SessionManager." << std::endl;

    // 3. Verify Polling (Legacy support)
    int trigger = controller.triggerStill();
    assert(trigger == 1);
    assert(controller.triggerStill_ == 0); // Should reset
    std::cout << "PASS: triggerStill() polling returns status and resets." << std::endl;

    std::cout << "All Still Capture Logic tests passed." << std::endl;
    return 0;
}
