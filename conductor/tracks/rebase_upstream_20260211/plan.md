# Implementation Plan - Rebase Upstream

## Phase 1: Preparation & Rebase
- [x] Task: Configure Remote and Fetch 1125bbe
    - [x] Add `upstream` remote pointing to `https://github.com/raspberrypi/rpicam-apps.git` (if not exists).
    - [x] Fetch latest changes from `upstream`.
    - [x] Create a backup branch (e.g., `main-backup-pre-rebase`) to ensure safety.
- [ ] Task: Execute Rebase
    - [ ] Checkout `main` branch.
    - [ ] Initiate `git rebase upstream/main`.
    - [ ] Identify files with conflicts.

## Phase 2: Conflict Resolution
- [ ] Task: Resolve Build Configuration Conflicts
    - [ ] Inspect `CMakeLists.txt` conflicts.
    - [ ] Re-integrate `cinepi` build instructions into the new upstream `CMakeLists.txt`.
    - [ ] Verify `package.cmake` or other build helpers if affected.
- [ ] Task: Resolve Core Application Conflicts
    - [ ] Inspect `core/libcamera_app.cpp` and `core/libcamera_app.hpp`.
    - [ ] Ensure `CinePIRecorder` inheritance and method overrides remain valid.
    - [ ] Resolve any conflicts in `core/options.cpp` regarding command-line arguments.
- [ ] Task: Resolve Remaining Conflicts
    - [ ] Address conflicts in `encoder/` or `output/` directories if `cinepi` relies on modified base classes.
    - [ ] Mark all conflicts as resolved (`git add`).
    - [ ] Complete the rebase (`git rebase --continue`).

## Phase 3: Verification & Cleanup
- [ ] Task: Build Verification
    - [ ] Run `cmake .` to regenerate build files.
    - [ ] Run `make -j$(nproc)` to compile the project.
    - [ ] Fix any compilation errors resulting from API mismatches (e.g., changed `libcamera` headers).
- [ ] Task: Functional Verification
    - [ ] Launch `cinepi-raw` (or the resulting binary) to verify it starts.
    - [ ] Perform a "Dry Run" test if possible (or user manual verification).
- [ ] Task: Conductor - User Manual Verification 'Verification & Cleanup' (Protocol in workflow.md)
