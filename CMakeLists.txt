add_executable(usb-relay-controler
    usb-relay-controler.c
    modules/cmd-interface.c
    )

# Pull in our pico_stdlib which pulls in commonly used features 
target_link_libraries(usb-relay-controler pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(usb-relay-controler 1)
#pico_enable_stdio_uart(diag-interface 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(usb-relay-controler)