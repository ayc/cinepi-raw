hiredis_dep = dependency('hiredis', required : true)
redis_plus_plus_dep = dependency('redis++', required : true)

cinepi_raw = executable('cinepi-raw', files('cinepi_raw.cpp', 'cinepi_controller.cpp', 'cinepi_state.cpp', 'dng_encoder.cpp', 'redis_control.cpp', 'dng_writer.cpp', 'processors/neon_unpacker.cpp', 'processors/lj92_compressor.cpp'),
                        include_directories : include_directories('..'),
                        dependencies: [libcamera_dep, boost_dep, hiredis_dep, redis_plus_plus_dep],
                        link_with : rpicam_app,
                        install : true)
