# Implementation Plan - Complete Redis Control Feature Parity

## Phase 1: Interface Expansion
- [x] Task: Update ControlInterface
    - [x] Write Tests: Update `utils/test_control_interface.cpp` to include new signals (`setWb`, `setColorGains`, `setShutterAngle`, `triggerStill`, etc.).
    - [x] Implement: Add new signals to `cinepi/control_interface.hpp`.
- [x] Task: Conductor - User Manual Verification 'Interface Expansion' (Protocol in workflow.md) (Skipped: Automated test verified)

## Phase 2: RedisControl Command Completion
- [ ] Task: Implement Missing Command Handlers
    - [ ] Write Tests: Create `test/test_redis_command_parsing.cpp` to verify parsing of complex strings (like ColorGains "1.0,2.0").
    - [ ] Implement: Update `RedisControl::handleMessage` in `cinepi/redis_control.cpp` to support all keys defined in the spec.
- [ ] Task: Implement Startup Synchronization
    - [ ] Write Tests: Verify `sync()` triggers all signals correctly based on pre-populated Redis keys.
    - [ ] Implement: Add `RedisControl::sync()` method to fetch all initial values and emit corresponding signals.
- [ ] Task: Conductor - User Manual Verification 'RedisControl Command Completion' (Protocol in workflow.md)

## Phase 3: Stats Aggregation & Final Polish
- [ ] Task: Implement JSON Stats in CinePIController
    - [ ] Write Tests: Verify JSON string format matches expectations.
    - [ ] Implement: Update `CinePIController::process` to build a JSON string and emit `publishStats`.
- [ ] Task: Implement Still Capture Logic
    - [ ] Write Tests: Verify `triggerStill` signal causes a still folder creation.
    - [ ] Implement: Add slot `onTriggerStill` to `CinePIController` and call `session_->ensureStillsFolder()`.
- [ ] Task: Final Integration Test
    - [ ] Write Tests: Verify end-to-end command flow from Redis -> RedisControl -> Interface -> Controller -> SessionManager.
    - [ ] Implement: Update `main()` to ensure everything is wired correctly.
- [ ] Task: Conductor - User Manual Verification 'Stats Aggregation & Final Polish' (Protocol in workflow.md)
