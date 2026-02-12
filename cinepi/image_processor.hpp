#pragma once

#include <cstdint>
#include <cstddef>
#include "core/stream_info.hpp"

class ImageProcessor
{
public:
    virtual ~ImageProcessor() = default;

    // Process the input buffer and write to the output buffer.
    // input: Source raw data from libcamera
    // output: Destination buffer (must be pre-allocated)
    // info: Stream info for the input
    // Returns: Number of bytes written to output
    virtual size_t process(uint8_t const *input, uint8_t *output, StreamInfo const &info) = 0;
};
