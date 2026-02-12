![cp_raw_banner](https://github.com/cinepi/cinepi-raw/assets/25234407/71591abc-f9b2-467e-806f-30557bcd1491)

# CinePi Raw

*Fork of rpicam-apps (formerly libcamera-apps) that builds upon the rpicam-raw app, offering CinemaDNG recording capabilities and integration with Redis offering an abstract "API" like layer for custom integrations.*

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

For advanced usage, refer to the official [Raspberry Pi documentation](https://www.raspberrypi.com/documentation/computers/camera_software.html#building-libcamera-and-rpicam-apps).

License
-------

The source code is made available under the simplified [BSD 2-Clause license](https://spdx.org/licenses/BSD-2-Clause.html).
