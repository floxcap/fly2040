# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE MinSizeRel)
add_compile_definitions(WREN_OPT_META=0)
add_compile_definitions(WREN_OPT_RANDOM=0)

# Pull in Raspberry Pi Pico SDK (must be before project)
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(usk C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(${PROJECT_SOURCE_DIR} ../wren/src/vm ../wren/src/include NanoCBOR/include ../../app/common/include)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.c ../wren/src/vm/*.c NanoCBOR/src/*.c)

add_executable(usk ${SRC_FILES})

target_compile_definitions(usk PRIVATE PICO_NO_BINARY_INFO)

pico_generate_pio_header(usk ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(usk ${CMAKE_CURRENT_LIST_DIR}/emmc.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

pico_set_program_name(usk "usk")
pico_set_program_version(usk "2.7")

pico_set_binary_type(usk no_flash)

pico_enable_stdio_uart(usk 0)
pico_enable_stdio_usb(usk 0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPICO_USE_MALLOC_MUTEX=1")
add_subdirectory(pico-littlefs-lib)

target_link_libraries(usk
        hardware_pio
        hardware_adc
        hardware_flash
        hardware_i2c
        pico_i2c_slave
        pico_stdlib
        littlefs-lib
        )

pico_add_extra_outputs(usk)

# Enable usb output, disable uart output
#pico_enable_stdio_usb(usk 1)
#pico_enable_stdio_uart(usk 0)