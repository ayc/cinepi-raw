#include "neon_unpacker.hpp"
#include <arm_neon.h>
#include <cstring>
#include <stdexcept>

// Helper function for NEON unpacking
static inline void unpack12p(uint8x16x3_t *input){
    uint8x16_t tmp1 = input->val[1];
    uint8x16_t tmp2 = input->val[2];

    input->val[1] = vorrq_u8(vshlq_n_u8(tmp2,4),vshrq_n_u8(tmp1,4));
    input->val[2] = vorrq_u8(vshlq_n_u8(tmp1,4),vshrq_n_u8(tmp2,4));
}

size_t NeonUnpacker::process(uint8_t const *input, uint8_t *output, StreamInfo const &info)
{
    // NEON UNPACK
    uint8x16x3_t nbuf;
    uint8x16x3_t nbuf1;
    uint8x16x3_t nbuf2;
    uint8x16x3_t nbuf3;

    for (unsigned int y = 0; y < info.height; y++)
    {
        // Output buffer row
        uint64_t* write_row = (uint64_t *)(output + y * info.width * 2); // 16-bit output = 2 bytes per pixel
        // Input buffer row
        uint64_t* read = (uint64_t *)(input + y * info.stride);

        // We process in chunks of 24 bytes (packed) -> 32 bytes (unpacked)
        // info.stride is bytes per row in packed format.
        // We need to iterate enough times to cover the width.
        // The original code used a fixed buffer on stack `uint64_t buffer[384];` which was scary.
        // Here we write directly to output.
        
        // Caution: The original code logic was:
        // for(uint64_t* write = buffer; write < buffer + info.stride/8; write += 24){ ... }
        // info.stride/8 is number of uint64_t in a row.
        // This loop logic seems tailored to specific stride/width.
        // We will adapt the original loop to write to 'output'.
        
        // Wait, the original code wrote to a temp buffer then used TIFFWriteScanline.
        // Here we want to write to the 'output' buffer which DngWriter will consume.
        
        // Original loop adaptation:
        uint64_t* write = write_row;
        uint64_t* row_end = write_row + (info.width * 2) / 8; // End of row in 64-bit words

        while(write < row_end) {
            nbuf = vld3q_u8((uint8_t const*)read);
            nbuf1 = vld3q_u8((uint8_t const*)read+48);
            nbuf2 = vld3q_u8((uint8_t const*)read+96);
            nbuf3 = vld3q_u8((uint8_t const*)read+144);
            unpack12p(&nbuf);
            unpack12p(&nbuf1);
            unpack12p(&nbuf2);
            unpack12p(&nbuf3);
            vst3q_u8((uint8_t *)write, nbuf);
            vst3q_u8((uint8_t *)write+48, nbuf1);
            vst3q_u8((uint8_t *)write+96, nbuf2);
            vst3q_u8((uint8_t *)write+144, nbuf3);
            read += 24; // 24 uint64_t * 8 bytes = 192 bytes read? No.
            // Original: read += 24. read is uint64_t*. 24 * 8 = 192 bytes.
            // write += 24. write is uint64_t*. 24 * 8 = 192 bytes?
            
            // NEON unpack expands 12 bits to 16 bits.
            // 12 bytes packed -> 16 bytes unpacked. Ratio 4:3.
            // vld3q_u8 loads 3 registers of 16 bytes = 48 bytes.
            // unpack12p processes them.
            // vst3q_u8 stores 48 bytes.
            
            // The original loop incremented pointers by 24 (uint64_t). 24 * 8 = 192 bytes.
            // 4 calls to vld3q_u8. Each loads 48 bytes. 4*48 = 192.
            // Checks out.
            
            write += 24;
        }
    }
    
    return info.width * info.height * 2; // Total bytes written (16-bit)
}
