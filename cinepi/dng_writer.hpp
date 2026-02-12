#pragma once

#include <string>
#include <vector>
#include <libcamera/controls.h>

#include "core/stream_info.hpp"
#include "raw_options.hpp"

class DngWriter
{
public:
    DngWriter(RawOptions const *options);
    virtual ~DngWriter() = default;

    // Writes a DNG file to disk.
    // mem: Pointer to the main image data (likely already unpacked/compressed).
    // info: Stream info for the main image.
    // lomem: Pointer to the thumbnail image data.
    // loinfo: Stream info for the thumbnail.
    // losize: Size of the thumbnail data.
    // metadata: Camera metadata for the frame.
    // filename: Output filename.
    // fn: Frame number (for timecode/sequence).
    void writeFrame(uint8_t const *mem, StreamInfo const &info,
                    uint8_t const *lomem, StreamInfo const &loinfo, size_t losize,
                    libcamera::ControlList const &metadata, std::string const &filename, uint64_t fn);

private:
    RawOptions const *options_;
};
