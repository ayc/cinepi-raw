# Track Specification: Update Build Documentation

## Goal
To provide clear, self-contained build instructions in `README.md` that reflect the project's migration to the Meson build system and its specific dependencies.

## Core Requirements
1.  **List Dependencies:** explicitly list all required packages: `libcamera-dev`, `libboost-program-options-dev`, `libhiredis-dev`, `libredis++-dev`, `libtiff-dev`, `meson`, and `ninja-build`.
2.  **Add Build Commands:** Provide the exact `meson` setup and compile commands.
3.  **Remove Ambiguity:** Replace the generic link to Raspberry Pi docs with actual steps, while keeping the link as a reference for advanced usage.

## Detailed Considerations
-   The instructions should target the Raspberry Pi OS (Debian-based) environment as the primary platform.
