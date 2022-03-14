cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(GameTiger VERSION 1.0.0)
 
add_executable(GameTiger 
    core/display.cpp
    core/image.cpp
    core/battery.cpp
    core/keyboard.cpp
    content/font.cpp
    splashscreen.cpp
    menuscreen.cpp
    snakescreen.cpp
    main.cpp
)

target_link_libraries(GameTiger pico_stdlib hardware_spi hardware_pwm hardware_adc hardware_dma hardware_flash hardware_sync)

pico_sdk_init()
pico_enable_stdio_usb(GameTiger 1)
pico_enable_stdio_uart(GameTiger 1)

pico_add_extra_outputs(GameTiger)

file(COPY ${CMAKE_CURRENT_BINARY_DIR}/GameTiger.uf2 DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
