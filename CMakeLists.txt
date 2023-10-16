cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_definitions ( -DFORMPU=1 )
add_definitions ( -DRP2040=1 )

set(PICO_SDK_PATH /Users/harishankar/Development/pico-sdk )
set(PICO_EXTRAS_PATH /Users/harishankar/Development/pico-extras )
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

project(GameTiger VERSION 1.0.0)
 
set(PICO_BOARD gametiger_rp2040)

add_executable(${PROJECT_NAME} 
    core/common.cpp
    core/LoRa/sx126x_hal.cpp
    core/LoRa/sx126x.cpp
    core/framebuffer.cpp
    core/display.cpp
    core/image.cpp
    core/battery.cpp
    core/keyboard.cpp
    core/tilemap.cpp
    core/level.cpp
    core/LoRa/lora.cpp
    screens/splashscreen.cpp
    screens/menuscreen.cpp
    screens/snakescreen.cpp
    screens/gameboyscreen.cpp
    screens/g2048screen.cpp
    screens/tetrisscreen.cpp
    screens/minescreen.cpp
    screens/ticscreen.cpp
    screens/aboutscreen.cpp
    screens/settingsscreen.cpp
    screens/pa2screen.cpp
    main.cpp
)

pico_sdk_init()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib 
    pico_multicore 
    hardware_spi 
    hardware_i2c 
    hardware_pwm 
    hardware_adc 
    hardware_dma 
    hardware_flash 
    hardware_sync
    hardware_pll
)
target_compile_definitions(${PROJECT_NAME} PRIVATE PICO_CLOCK_AJDUST_PERI_CLOCK_WITH_SYS_CLOCK=1)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
