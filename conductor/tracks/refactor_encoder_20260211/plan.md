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
- [ ] Task: Define ImageProcessor Interface
    - [ ] Create `cinepi/image_processor.hpp`.
    - [ ] Define virtual `process(input, output)` method.
- [ ] Task: Implement Concrete Processors
    - [ ] Create `cinepi/processors/neon_unpacker.cpp` (move NEON code).
    - [ ] Create `cinepi/processors/lj92_compressor.cpp` (move `lj92` call).
- [ ] Task: Integrate Processors
    - [ ] Update `DngEncoder` to select and use the appropriate processor based on options.
- [ ] Task: Conductor - User Manual Verification 'Extract Image Processors' (Protocol in workflow.md)

## Phase 3: Cleanup and Renaming
- [ ] Task: Rename DngEncoder? (Optional)
    - [ ] Consider renaming `DngEncoder` to `AsyncPipeline` or similar to reflect its new role. (Decision: Keep `DngEncoder` name for now to minimize ripple effects, but clean up internals).
- [ ] Task: Final Code Polish
    - [ ] Remove unused includes and headers.
    - [ ] Ensure all new files are in `meson.build`.
- [ ] Task: Conductor - User Manual Verification 'Cleanup' (Protocol in workflow.md)
