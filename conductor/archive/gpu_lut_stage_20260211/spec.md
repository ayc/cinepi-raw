# Track Specification: GPU LUT Post-Processing Stage

## Overview
This track aims to implement a new post-processing stage for `rpicam-apps` called `LutStage`. This stage will leverage the Raspberry Pi's GPU (via OpenGL ES 3.1) to apply high-quality 3D Look Up Tables (.cube files) to the image stream in real-time. This allows users to "bake in" cinematic looks into standard video (H.264) or still (JPEG) outputs.

## Functional Requirements
1.  **New Post-Processing Stage:** Create `LutStage` within the `post_processing_stages/` directory.
2.  **GPU Acceleration (OpenGL ES 3.1):**
    *   Initialize an EGL context for background GPU processing.
    *   Load 3D LUTs from `.cube` files (supporting 33^3 and 64^3 sizes).
    *   Implement a fragment shader to map input RGB/YUV colors to 3D LUT coordinates and output the corrected color.
3.  **Zero-Copy Integration:**
    *   Use DMA-BUF sharing to import camera buffers directly into OpenGL textures without CPU-side copying.
4.  **Flexible Configuration:**
    *   Load parameters (LUT path, strength, toggle state) via the standard post-processing JSON configuration.
5.  **Multi-Format Support:**
    *   Handle common ISP output formats (NV12, RGB) natively within the shader.

## Non-Functional Requirements
1.  **Performance:** Target real-time performance (24fps+) for 1080p and 4K resolutions on Raspberry Pi 4/5.
2.  **Modularity:** Follow the existing `PostProcessingStage` base class pattern to ensure compatibility with `rpicam-vid` and `rpicam-still`.
3.  **Stability:** Ensure the GPU context is correctly managed and doesn't leak memory or crash during long recording sessions.

## Acceptance Criteria
1.  A new `rpicam-apps-lut` shared library is built and can be loaded as a post-processing stage.
2.  When a `.cube` file is loaded via JSON config, the output video/image shows the expected color transformation.
3.  The feature can be enabled/disabled via a JSON parameter without restarting the app.
4.  Performance logs show minimal overhead (target < 10ms processing time per frame).

## Out of Scope
-   Writing a GUI for LUT selection (handled via command-line JSON config).
-   Modifying the CinePi CinemaDNG pipeline (this track is for standard baked formats).
