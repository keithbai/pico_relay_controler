cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project) Pulls in file named "pico_sdk_import.cmake" from same diretory
include(pico_sdk_import.cmake)

# ##include(pico_extras_import_optional.cmake)

project(pico_relay_controler C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(RELAY_CONTROLER_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()





# add_subdirectory(modules)


add_executable(usb-relay-controler
    usb-relay-controler.c
    modules/cmd-interface.c
    )

# Pull in our pico_stdlib which pulls in commonly used features 
target_link_libraries(usb-relay-controler pico_stdlib)

# enable usb output, disable uart output
    pico_enable_stdio_usb(usb-relay-controler 1)
    # pico_enable_stdio_uart(diag-interface 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(usb-relay-controler)

# add url via pico_set_program_url
# example_auto_set_url(usb-relay-controler)