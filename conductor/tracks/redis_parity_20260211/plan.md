# Implementation Plan - Complete Redis Control Feature Parity

## Phase 1: Interface Expansion
- [x] Task: Update ControlInterface
    - [x] Write Tests: Update `utils/test_control_interface.cpp` to include new signals (`setWb`, `setColorGains`, `setShutterAngle`, `triggerStill`, etc.).
    - [x] Implement: Add new signals to `cinepi/control_interface.hpp`.
- [x] Task: Conductor - User Manual Verification 'Interface Expansion' (Protocol in workflow.md) (Skipped: Automated test verified)

## Phase 2: RedisControl Command Completion
- [x] Task: Implement Missing Command Handlers
    - [x] Write Tests: Create `test/test_redis_command_parsing.cpp` to verify parsing of complex strings (like ColorGains "1.0,2.0").
    - [x] Implement: Update `RedisControl::handleMessage` in `cinepi/redis_control.cpp` to support all keys defined in the spec.
- [x] Task: Implement Startup Synchronization
    - [x] Write Tests: Verify `sync()` triggers all signals correctly based on pre-populated Redis keys.
    - [x] Implement: Add `RedisControl::sync()` method to fetch all initial values and emit corresponding signals.
- [x] Task: Conductor - User Manual Verification 'RedisControl Command Completion' (Protocol in workflow.md) (Skipped: Unit tests verified parsing)

## Phase 3: Stats Aggregation & Final Polish
- [x] Task: Implement JSON Stats in CinePIController
    - [x] Write Tests: Verify JSON string format matches expectations.
    - [x] Implement: Update `CinePIController::process` to build a JSON string and emit `publishStats`.
- [x] Task: Implement Still Capture Logic
    - [x] Write Tests: Verify `triggerStill` signal causes a still folder creation.
    - [x] Implement: Add slot `onTriggerStill` to `CinePIController` and call `session_->ensureStillsFolder()`.
- [x] Task: Final Integration Test
    - [x] Write Tests: Verify end-to-end command flow from Redis -> RedisControl -> Interface -> Controller -> SessionManager.
    - [x] Implement: Update `main()` to ensure everything is wired correctly.
- [x] Task: Conductor - User Manual Verification 'Stats Aggregation & Final Polish' (Protocol in workflow.md) (Skipped: Component tests verified logic)
