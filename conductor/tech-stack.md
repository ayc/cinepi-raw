# Technology Stack

## Core Technologies
- **C++17:** The primary programming language used for high-performance camera control and raw frame processing.
- **CMake:** The build system for managing dependencies and generating build files across Raspberry Pi OS distributions.
- **libcamera:** The underlying Linux camera framework used for sensor abstraction and image signal processing (ISP).
- **rpicam-apps (Upstream):** The foundation framework providing the application structure, encoder interfaces, and camera management.

## Integration & Communication
- **Redis:** Used as a message broker and state store for remote control.
    - **hiredis:** Minimalist C client library for Redis.
    - **redis-plus-plus:** Modern C++ wrapper for hiredis used for higher-level abstraction.

## Target Environment
- **Platform:** Raspberry Pi (ARMhf/ARM64).
- **OS:** Raspberry Pi OS (Bullseye/Bookworm).
- **Hardware:** Raspberry Pi Camera Modules (v1, v2, v3, HQ, Global Shutter).

## Future Considerations
- **Swappable Control Layer:** Plans to abstract the Redis integration to support alternative protocols (e.g., gRPC, REST, or direct IPC) for app-based control.
