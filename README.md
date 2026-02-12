![cp_raw_banner](https://github.com/cinepi/cinepi-raw/assets/25234407/71591abc-f9b2-467e-806f-30557bcd1491)

# CinePi Raw

*Fork of rpicam-apps (formerly libcamera-apps) that builds upon the rpicam-raw app, offering CinemaDNG recording capabilities and a modular control layer that supports multiple interfaces (starting with Redis) for custom integrations/remote control.*

### Build Instructions

1. **Install Dependencies:**
   ```bash
   sudo apt update
   sudo apt install -y libcamera-dev libboost-program-options-dev libhiredis-dev libredis++-dev libtiff-dev meson ninja-build
   ```

2. **Setup and Compile:**
   ```bash
   meson setup build
   meson compile -C build
   ```

### Using 3D LUTs (GPU Accelerated)

CinePi Raw supports applying real-time 3D Look Up Tables (LUTs) to the video stream using the GPU. This is implemented as a post-processing stage.

**Configuration:**
To use a LUT, create a JSON configuration file (e.g., `lut_config.json`) with the following structure:

```json
{
    "lut": {
        "file": "/path/to/your/look.cube",
        "strength": 1.0,
        "enabled": 1
    }
}
```

*   **file:** Absolute path to a standard `.cube` 3D LUT file (33x33x33 and 64x64x64 supported).
*   **strength:** Blending factor between 0.0 (original) and 1.0 (fully processed).
*   **enabled:** Set to 1 to enable, 0 to disable.

**Running with LUT:**
Pass the configuration file to the application using the `--post-process-file` argument. This works with both `cinepi-raw` and standard `rpicam-apps`:

```bash
# Apply LUT to standard video recording (Baked H.264)
rpicam-vid -t 10000 -o test.h264 --post-process-file lut_config.json

# Use with cinepi-raw
./build/cinepi/cinepi-raw --post-process-file lut_config.json
```

For advanced usage, refer to the official [Raspberry Pi documentation](https://www.raspberrypi.com/documentation/computers/camera_software.html#building-libcamera-and-rpicam-apps).

License
-------

The source code is made available under the simplified [BSD 2-Clause license](https://spdx.org/licenses/BSD-2-Clause.html).