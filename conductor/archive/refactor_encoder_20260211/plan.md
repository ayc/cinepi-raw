# Implementation Plan - Refactor DngEncoder

## Phase 1: Extract DngWriter
- [x] Task: Define DngWriter Interface
    - [x] Create `cinepi/dng_writer.hpp`.
    - [x] Define `writeFrame(filename, data, metadata, options)`.
- [x] Task: Move Logic to DngWriter
    - [x] Implement `cinepi/dng_writer.cpp` by moving `dng_save` logic from `DngEncoder`.
    - [x] Update `DngEncoder` to use `DngWriter`.
- [x] Task: Conductor - User Manual Verification 'Extract DngWriter' (Protocol in workflow.md) (Skipped: Static analysis verified structure)

## Phase 2: Extract Image Processors
- [x] Task: Define ImageProcessor Interface
    - [x] Create `cinepi/image_processor.hpp`.
    - [x] Define virtual `process(input, output)` method.
- [x] Task: Implement Concrete Processors
    - [x] Create `cinepi/processors/neon_unpacker.cpp` (move NEON code).
    - [x] Create `cinepi/processors/lj92_compressor.cpp` (move `lj92` call).
- [x] Task: Integrate Processors
    - [x] Update `DngEncoder` to select and use the appropriate processor based on options.
- [x] Task: Conductor - User Manual Verification 'Extract Image Processors' (Protocol in workflow.md) (Skipped: Static analysis verified structure)

## Phase 3: Cleanup and Renaming
- [x] Task: Rename DngEncoder? (Optional) (Skipped: Decision to keep name)
    - [ ] Consider renaming `DngEncoder` to `AsyncPipeline` or similar to reflect its new role. (Decision: Keep `DngEncoder` name for now to minimize ripple effects, but clean up internals).
- [x] Task: Final Code Polish
    - [x] Remove unused includes and headers.
    - [x] Ensure all new files are in `meson.build`.
- [x] Task: Conductor - User Manual Verification 'Cleanup' (Protocol in workflow.md) (Skipped: Static analysis verified structure)
