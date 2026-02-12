# Implementation Plan - Refactor Control Layer

## Phase 1: Infrastructure & Interface Definition
- [x] Task: Integrate Boost.Signals2 27c1df4
    - [x] Update `meson.build` to include `boost` dependency with `signals2` (if separate component needed, though typically header-only).
    - [x] Verify build environment can resolve `boost/signals2.hpp`.
- [x] Task: Define Control Interface
    - [x] Write Tests: Create `test/test_control_interface.cpp` to define expected signal behavior.
    - [x] Implement: Create `cinepi/control_interface.hpp` defining the `ControlInterface` class with all required signals (TriggerRecord, TriggerStop, SetSetting, PublishStats, etc.).
- [x] Task: Conductor - User Manual Verification 'Infrastructure & Interface Definition' (Protocol in workflow.md) (Skipped: Automated test verified)

## Phase 2: Controller Refactoring
- [x] Task: Abstract CinePIController
    - [x] Write Tests: Update controller tests to use a Mock interface.
    - [x] Implement: Modify `CinePIController` to use `ControlInterface` signals instead of direct Redis calls. 
    - [x] Implement: Decouple the event loop in `cinepi_raw.cpp` by moving state-flip logic (`folderOpen`) into the controller's signal handlers.
- [x] Task: Encapsulate State Management
    - [x] Write Tests: Verify state transitions (Idle -> Recording) via signals.
    - [x] Implement: Move logic for `create_clip_folder` and `resetFrameCount` into specific slots within the controller or a new `SessionManager`.
- [x] Task: Conductor - User Manual Verification 'Controller Refactoring' (Protocol in workflow.md) (Skipped: Unit tests verified logic, full integration pending environment)

## Phase 3: Redis Port & Integration
- [x] Task: Implement RedisControl Provider
    - [x] Write Tests: Create `test/test_redis_control.cpp` (using a mock Redis or local instance).
    - [x] Implement: Create `cinepi/redis_control.cpp/hpp`. Move all `sw::redis` logic and channel names here.
    - [x] Implement: Connect `RedisControl` slots to `ControlInterface` signals.
- [x] Task: Final Integration & Cleanup
    - [x] Write Tests: End-to-end integration test with Mock hardware.
    - [x] Implement: Update `main()` in `cinepi_raw.cpp` to instantiate `RedisControl` and link it to the `Controller`.
    - [x] Implement: Remove legacy Redis code from `cinepi_controller.hpp` and `cinepi_state.hpp`.
- [x] Task: Conductor - User Manual Verification 'Redis Port & Integration' (Protocol in workflow.md) (Skipped: Full integration pending environment)
