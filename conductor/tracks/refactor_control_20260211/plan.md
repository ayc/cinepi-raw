# Implementation Plan - Refactor Control Layer

## Phase 1: Infrastructure & Interface Definition
- [x] Task: Integrate Boost.Signals2 27c1df4
    - [x] Update `meson.build` to include `boost` dependency with `signals2` (if separate component needed, though typically header-only).
    - [x] Verify build environment can resolve `boost/signals2.hpp`.
- [ ] Task: Define Control Interface
    - [ ] Write Tests: Create `test/test_control_interface.cpp` to define expected signal behavior.
    - [ ] Implement: Create `cinepi/control_interface.hpp` defining the `ControlInterface` class with all required signals (TriggerRecord, TriggerStop, SetSetting, PublishStats, etc.).
- [ ] Task: Conductor - User Manual Verification 'Infrastructure & Interface Definition' (Protocol in workflow.md)

## Phase 2: Controller Refactoring
- [ ] Task: Abstract CinePIController
    - [ ] Write Tests: Update controller tests to use a Mock interface.
    - [ ] Implement: Modify `CinePIController` to use `ControlInterface` signals instead of direct Redis calls. 
    - [ ] Implement: Decouple the event loop in `cinepi_raw.cpp` by moving state-flip logic (`folderOpen`) into the controller's signal handlers.
- [ ] Task: Encapsulate State Management
    - [ ] Write Tests: Verify state transitions (Idle -> Recording) via signals.
    - [ ] Implement: Move logic for `create_clip_folder` and `resetFrameCount` into specific slots within the controller or a new `SessionManager`.
- [ ] Task: Conductor - User Manual Verification 'Controller Refactoring' (Protocol in workflow.md)

## Phase 3: Redis Port & Integration
- [ ] Task: Implement RedisControl Provider
    - [ ] Write Tests: Create `test/test_redis_control.cpp` (using a mock Redis or local instance).
    - [ ] Implement: Create `cinepi/redis_control.cpp/hpp`. Move all `sw::redis` logic and channel names here.
    - [ ] Implement: Connect `RedisControl` slots to `ControlInterface` signals.
- [ ] Task: Final Integration & Cleanup
    - [ ] Write Tests: End-to-end integration test with Mock hardware.
    - [ ] Implement: Update `main()` in `cinepi_raw.cpp` to instantiate `RedisControl` and link it to the `Controller`.
    - [ ] Implement: Remove legacy Redis code from `cinepi_controller.hpp` and `cinepi_state.hpp`.
- [ ] Task: Conductor - User Manual Verification 'Redis Port & Integration' (Protocol in workflow.md)
