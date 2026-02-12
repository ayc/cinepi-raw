# Track Specification: Refactor DngEncoder

## Overview
The `DngEncoder` class currently acts as a "God Object," handling buffer management, concurrency, image processing (NEON unpacking, compression), and DNG file formatting simultaneously. This track will refactor it into smaller, single-responsibility components to improve testability, maintainability, and flexibility.

## Functional Requirements
1.  **Extract `DngWriter`:** Create a class dedicated solely to formatting and writing DNG files using `libtiff`. It should accept pre-processed image data and metadata.
2.  **Extract `ImageProcessor` Strategy:** Create an abstract interface for image processing, with concrete implementations for:
    *   `IdentityProcessor` (or `UnpackingProcessor`): Handles standard NEON unpacking.
    *   `Lj92Compressor`: Handles Lossless JPEG compression.
3.  **Refactor `DngEncoder` to `AsyncEncoder`:** Transform the original class into a coordinator (likely renamed or kept as a facade) that manages the `FrameQueue` and delegates work to the `ImageProcessor` and `DngWriter`.

## Non-Functional Requirements
1.  **Performance:** The refactor must not introduce significant overhead. The copy-then-process threading model must be preserved (or improved).
2.  **Compatibility:** The external API exposed to `CinePIRecorder` (`EncodeBuffer2`) should remain largely unchanged or have a compatible adapter.

## Acceptance Criteria
1.  `DngWriter` exists and contains all `libtiff` logic.
2.  `dng_encoder.cpp` no longer contains direct `TIFF*` calls or NEON intrinsics (moved to processors).
3.  Unit tests exist for `DngWriter` (mocking the filesystem if possible, or just verifying API) and `ImageProcessor`.
4.  The application still compiles and runs (verified via build check).

## Out of Scope
-   Changing the underlying threading model (e.g., switching to a task stealing pool) - we are just moving the existing logic into better homes.
