cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(FORCPU)
    find_package(SDL2 REQUIRED)
else(FORCPU)
    add_definitions ( -DFORMPU=1 )

    set(PICO_BOARD adafruit_feather_rp2040)
    set(PICO_SDK_PATH /Users/harishankar/Development/pico-sdk )
    set(PICO_EXTRAS_PATH /Users/harishankar/Development/pico-extras )
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR ARM)

    include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
    include(${PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

endif(FORCPU)

project(GameTiger VERSION 1.0.0)
 
add_executable(${PROJECT_NAME} 
    core/common.cpp
    core/display.cpp
    core/image.cpp
    core/battery.cpp
    core/keyboard.cpp
    core/tilemap.cpp
    core/level.cpp
    core/audio.cpp
    core/SX126x.cpp
    core/lora.cpp
    screens/splashscreen.cpp
    screens/menuscreen.cpp
    screens/snakescreen.cpp
    screens/g2048screen.cpp
    screens/tetrisscreen.cpp
    screens/minescreen.cpp
    screens/ticscreen.cpp
    screens/aboutscreen.cpp
    screens/settingsscreen.cpp
    screens/pa2screen.cpp
    main.cpp
)

if(FORCPU)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
else(FORCPU)
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
    )
endif(FORCPU)

if(FORCPU)
else(FORCPU)
    pico_sdk_init()
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 1)

    pico_add_extra_outputs(${PROJECT_NAME})
endif(FORCPU)