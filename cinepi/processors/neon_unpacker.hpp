#pragma once

#include "cinepi/image_processor.hpp"

class NeonUnpacker : public ImageProcessor
{
public:
    size_t process(uint8_t const *input, uint8_t *output, StreamInfo const &info) override;
};
