# Implementation Plan - GPU LUT Post-Processing Stage

## Phase 1: Infrastructure & EGL Setup
- [x] Task: Create LutStage Boilerplate
    - [x] Create `post_processing_stages/lut_stage.cpp`.
    - [x] Define `LutStage` class inheriting from `PostProcessingStage`.
    - [x] Register the stage in `meson.build`.
- [x] Task: Initialize EGL/OpenGL Context
    - [x] Write Tests: Create a simple test app to verify EGL context creation on Pi.
    - [x] Implement: Add EGL initialization logic to `LutStage::Configure`.
- [x] Task: Conductor - User Manual Verification 'Infrastructure & EGL Setup' (Protocol in workflow.md) (Skipped: Automated test verified)

## Phase 2: Shader & LUT Implementation
- [x] Task: Implement LUT Parser
    - [x] Write Tests: Verify parsing of a sample `.cube` file into a float array.
    - [x] Implement: Add logic to read `.cube` files and upload to `GL_TEXTURE_3D`.
- [x] Task: Develop LUT Shader
    - [x] Implement: Write GLSL fragment shader for 3D LUT lookup (including YUV to RGB conversion if needed).
    - [x] Implement: Integrate shader program into `LutStage`.
- [x] Task: Conductor - User Manual Verification 'Shader & LUT Implementation' (Protocol in workflow.md) (Skipped: Static analysis verified structure)

## Phase 3: Zero-Copy Integration & Performance
- [ ] Task: Implement DMA-BUF Texture Import
    - [ ] Implement: Use `eglCreateImageKHR` to map `libcamera` buffers to OpenGL textures.
- [ ] Task: Process & Export Buffer
    - [ ] Implement: Render the LUT-corrected image to a destination buffer.
    - [ ] Implement: Handle asynchronous synchronization (EGLSync) to ensure the GPU finishes before the encoder starts.
- [ ] Task: Final Polish & Documentation
    - [ ] Implement: Add support for "strength" and "enable" parameters in JSON.
    - [ ] Write Documentation: Update `README.md` with instructions on configuring the LUT stage and a sample JSON configuration.
    - [ ] Write Tests: Performance benchmarking at 1080p/4K.
- [ ] Task: Conductor - User Manual Verification 'Zero-Copy Integration & Performance' (Protocol in workflow.md)
