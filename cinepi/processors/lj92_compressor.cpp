#include "lj92_compressor.hpp"
#include "cinepi/lj92.h"
#include <stdexcept>
#include <cstring>
#include <cstdlib>

size_t Lj92Compressor::process(uint8_t const *input, uint8_t *output, StreamInfo const &info)
{
    // LJ92 START
    uint8_t *encoded = NULL;
    int encodedLength;
    int w, h;
    w = info.stride / 2;
    h = info.height;
    
    // Note: lj92_encode allocates memory for 'encoded'. We need to copy it to 'output' and free it.
    // Ideally we'd modify lj92_encode to write directly to output, but for now we follow existing pattern.
    
    int ret = lj92_encode((uint16_t*)input, w*2, (h/2), 16, w*h, 0, NULL, 0, &encoded, &encodedLength);
    
    if(ret == LJ92_ERROR_NONE){
        if (output) {
            memcpy(output, encoded, encodedLength);
        }
        free(encoded);
        return encodedLength;
    } else {
        free(encoded); // Ensure we free even on error if it was allocated
        throw std::runtime_error("LJ92 Failed!");
    }
}
