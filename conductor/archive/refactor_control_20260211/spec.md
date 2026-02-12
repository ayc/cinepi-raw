# Track Specification: Refactor Control Layer

## Overview
This track aims to refactor the currently "hacky" and tightly coupled Redis integration in `CinePIController` into a decoupled, event-driven architecture using a Signal/Slot mechanism (specifically `boost::signals2`). This will allow the camera logic to remain agnostic of the underlying communication protocol, enabling future support for alternative interfaces (GUI apps, gRPC, etc.).

## Functional Requirements
1.  **Extract Control Interface:** Define a clean interface (likely a set of signals) that decouples the camera's core logic from the communication layer.
2.  **Implement Signal/Slot Architecture:**
    *   Use `boost::signals2` to handle communication between the Controller and the interface implementation.
    *   **Inbound Signals:** `TriggerRecord`, `TriggerStop`, `SetSetting` (ISO, Shutter, FrameRate).
    *   **Outbound Signals:** `PublishStats` (FPS, buffer usage), `PublishState` (Recording, Idle), `SystemWarning` (Disk, Thermal).
3.  **Port Redis to New Interface:** Re-implement the existing Redis logic as a concrete "slot" provider that connects to these signals.
4.  **Enforce Encapsulation:** Move all Redis-specific code out of `cinepi_controller.hpp` and into a implementation-specific file (e.g., `redis_control.cpp`).

## Non-Functional Requirements
1.  **Maintainability:** Eliminate the "hacky" queue assumptions and direct public member access currently used in the event loop.
2.  **Performance:** Ensure the signal/slot overhead is negligible compared to the 1ms-range processing requirements of the camera loop.
3.  **Testability:** The new architecture must allow for a `MockControl` implementation to be used in unit tests without requiring a running Redis server.

## Acceptance Criteria
1.  `CinePIController` no longer contains direct calls to `sw::redis` or Redis-specific channel names.
2.  The project compiles successfully with the new `boost::signals2` dependency.
3.  The camera still responds to Redis commands and publishes stats exactly as before (verified by manual Redis test).
4.  No performance regressions in frame processing or DNG writing.

## Out of Scope
-   Implementing new control protocols (gRPC, etc.) in this track.
-   Refactoring the `DngEncoder` logic.
