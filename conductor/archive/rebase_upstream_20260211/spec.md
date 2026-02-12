# Track Specification: Rebase Upstream

## Goal
To synchronize the local `cinepi-raw` fork with the official `rpicam-apps` upstream repository, ensuring the project benefits from the latest fixes, performance improvements, and hardware support while preserving custom CinemaDNG functionality.

## Core Requirements
1.  **Upstream Synchronization:**
    -   Fetch the latest changes from `https://github.com/raspberrypi/rpicam-apps.git`.
    -   Rebase the `main` branch onto the upstream `main` branch.

2.  **Conflict Resolution:**
    -   Identify and resolve all merge conflicts.
    -   **Critical Areas:** Pay special attention to `CMakeLists.txt`, `core/libcamera_app.cpp`, and any shared encoder logic where `cinepi` modifications might clash with upstream refactors.
    -   **Preservation:** Ensure `cinepi/` directory and its contents are strictly preserved.

3.  **Verification:**
    -   The project must compile successfully after the rebase.
    -   Existing tests (if any) and basic functionality (recording a clip) must be verified.

## Detailed Considerations
-   **CMake Changes:** Upstream often refactors build scripts. Ensure our custom `add_subdirectory(cinepi)` and linked libraries remain correctly configured.
-   **API Changes:** If `libcamera` APIs used by `LibcameraApp` have changed, update `CinePIRecorder` and `CinePIController` to match the new signatures.
