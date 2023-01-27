# CinePi Raw / Libcamera Apps

## Project Overview

**CinePi Raw** is a specialized fork of `rpicam-apps` (formerly `libcamera-apps`) designed for high-quality CinemaDNG raw video recording on Raspberry Pi hardware.

Key features include:
-   **CinemaDNG Recording:** specialized pipeline for writing raw frames to disk.
-   **Redis Integration:** A unique control layer that exposes camera state and accepts commands via Redis channels (`cp_controls`, `cp_stats`), enabling remote control interfaces.
-   **Architecture:** Built on top of the `libcamera` framework, utilizing the `LibcameraApp` base class for camera management.

## Directory Structure

*   **`cinepi/`**: Core CinePi specific logic.
    *   `cinepi_raw.cpp`: Main application entry point and event loop.
    *   `cinepi_controller.hpp/cpp`: Manages Redis communication, state, and recording triggers.
    *   `dng_encoder.cpp`: Handles the encoding/writing of DNG frames.
*   **`core/`**: Shared foundation libraries from `rpicam-apps`.
    *   `libcamera_app.cpp`: Base class wrapping `libcamera` functionality.
    *   `options.cpp`: Command-line and configuration parsing.
*   **`apps/`**: Standard `libcamera` applications (`libcamera_vid`, `libcamera_hello`, etc.).
*   **`encoder/`**: Video encoders (H.264, MJPEG, LibAV).
*   **`post_processing_stages/`**: Image processing stages (motion detect, HDR, etc.).

## Build Instructions

**Prerequisites:**
*   `libcamera` (installed via system packages or built from source).
*   Redis and C++ Redis clients: `Redis`, `Hiredis`, `Redis++`.

**Build Steps:**
```bash
# Configure the project
cmake .

# Build using all available cores
make -j$(nproc)
```

**Common Build Flags (`CMakeLists.txt`):**
*   `ENABLE_COMPILE_FLAGS_FOR_TARGET`: Automatically detected (e.g., `arm64`, `armv8-neon`).
*   `BUILD_SHARED_LIBS`: Default `ON`.

## Development Conventions

*   **Language:** C++17.
*   **Style:** Adheres to `libcamera` coding standards. See `.clang-format` and `utils/checkstyle.py`.
*   **Architecture:**
    *   **Event Loop:** The application runs a main event loop (`event_loop` in `cinepi_raw.cpp`) that waits for camera requests to complete.
    *   **Controller Pattern:** `CinePIController` decouples the camera logic from the external control interface (Redis).
    *   **Signals:** Uses `boost::signals2` or similar callback mechanisms (std::bind) for handling "Request Complete" or "Metadata Ready" events.

## Usage

**Running CinePi Raw:**
The main binary is likely built as `cinepi-raw` (or similar, depending on exact cmake target names which are dynamically generated).

```bash
./cinepi-raw [options]
```
*Note: Ensure the Redis server is running before starting the application.*
