# Product Definition

## Initial Concept
To create a high-quality, maintainable CinemaDNG recording application for Raspberry Pi that remains easily updatable with the official `rpicam-apps` upstream.

## Target Audience
- **Digital Cinema Hobbyists & Indie Filmmakers:** Users seeking affordable, high-quality raw video recording tools on Raspberry Pi.
- **Embedded Systems Developers:** Engineers building custom camera solutions requiring low-level control and raw data access.
- **Researchers:** Scientists and analysts requiring raw sensor data for computer vision and imaging experiments.

## Core Goals
- **Maintainability & Modularity:** The primary goal is to maintain a codebase that is easy to update and rebase against the upstream `rpicam-apps` repository.
- **Extensible Architecture:** Refactor existing integrations to ensure components (specifically the control layer) can be swapped or upgraded without deep coupling to the core logic.

## Key Features
- **CinemaDNG Recording:** A robust and reliable pipeline for writing raw DNG sequences with accurate metadata.
- **Streamlined Maintenance:** A build and project structure optimized for tracking official Raspberry Pi camera stack updates with minimal friction.
- **Abstracted Control Interface:** A refactored control layer that currently supports Redis but is designed to be swappable, paving the way for future app control interfaces.

## Architectural Guidelines
- **Isolation of Custom Logic:** The `cinepi/` directory and custom encoder logic should remain distinct to minimize conflicts during upstream merges.
- **Build Configuration Management:** `CMakeLists.txt` modifications should be managed carefully to preserve compatibility with upstream changes while supporting custom modules.
- **Decoupled Control Layer:** The Redis integration must be refactored into a generic interface, allowing for alternative control mechanisms to be implemented in the future.
