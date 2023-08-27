# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(WREN_OPT_META=0)
add_compile_definitions(WREN_OPT_RANDOM=0)

# Pull in Raspberry Pi Pico SDK (must be before project)
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(flyOS C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

pico_define_boot_stage2(slower_boot2 ${PICO_DEFAULT_BOOT_STAGE2_FILE})
target_compile_definitions(slower_boot2 PRIVATE PICO_FLASH_SPI_CLKDIV=4)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPICO_USE_MALLOC_MUTEX=1")

include_directories(${PROJECT_SOURCE_DIR} wren/src/vm wren/src/include NanoCBOR/include)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.c wren/src/vm/*.c NanoCBOR/src/*.c)

add_executable(flyOS ${SRC_FILES})

#set_target_properties(test PROPERTIES PICO_TARGET_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/memmap_custom.ld)
#pico_set_linker_script(TARGET SCRIPT)
target_compile_definitions(flyOS PRIVATE PICO_NO_BINARY_INFO)

pico_set_program_name(flyOS "flyOS")
pico_set_program_version(flyOS "1.0")

pico_set_binary_type(flyOS default)

pico_enable_stdio_uart(flyOS 0)
pico_enable_stdio_usb(flyOS 0)

add_subdirectory(pico-littlefs-lib)

# Add the standard library to the build
target_link_libraries(flyOS
        PUBLIC
        pico_stdlib
        littlefs-lib
        tinyusb_host
        tinyusb_board
)

pico_set_boot_stage2(flyOS slower_boot2)

pico_add_extra_outputs(flyOS)
