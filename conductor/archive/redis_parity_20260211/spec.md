# Track Specification: Complete Redis Control Feature Parity

## Overview
This track aims to complete the refactored control layer by implementing all missing Redis commands and restoring full status broadcasting. The goal is to reach feature parity with the original `CinePIController` while utilizing the new decoupled `ControlInterface` and `RedisControl` architecture.

## Functional Requirements
1.  **Command Completion:** Implement all missing inbound command handlers in `RedisControl`:
    *   `CONTROL_KEY_WB` (AWB Enable/Disable)
    *   `CONTROL_KEY_COLORGAINS` (Manual WB gains)
    *   `CONTROL_KEY_SHUTTER_ANGLE` / `CONTROL_KEY_SHUTTER_SPEED`
    *   `CONTROL_KEY_FRAMERATE`
    *   `CONTROL_KEY_WIDTH` / `CONTROL_KEY_HEIGHT`
    *   `CONTROL_KEY_COMPRESSION`
    *   `LV_KEY_ZOOM` (Liveview Zoom/Crop)
2.  **Startup Synchronization:**
    *   Implement initial state synchronization in `RedisControl`. At startup, it should fetch all control keys from Redis and trigger the corresponding `ControlInterface` signals to initialize the camera.
3.  **JSON Stats Broadcasting:**
    *   Implement a stats aggregation mechanism in `CinePIController`.
    *   Emit the `publishStats` signal once per frame with a JSON-formatted string containing: `framerate`, `colorTemp`, `focus`, `frameCount`, `bufferSize`.
    *   Update `RedisControl` to publish this JSON string to the `cp_stats` channel.
4.  **Still Capture Trigger:**
    *   Add `triggerStill` signal to `ControlInterface`.
    *   Implement logic in `RedisControl` to handle `CONTROL_TRIGGER_STILL`.
    *   Implement slot in `CinePIController` to trigger still capture via `SessionManager`.

## Non-Functional Requirements
1.  **Maintainability:** Use a modern C++ approach for JSON generation (e.g., a simple `stringstream` or a lightweight JSON header if available).
2.  **Performance:** Initial sync should be performed in the `RedisControl` start phase, not in the main camera loop.

## Acceptance Criteria
1.  All commands that worked in the original version are functional via Redis.
2.  Camera starts with settings persisted in Redis (if available).
3.  Status channel (`cp_stats`) receives JSON-formatted updates.
4.  Still captures are correctly triggered and saved to the `/stills` folder.

## Out of Scope
-   Refactoring the DNG writing performance.
-   Adding new UI elements.
