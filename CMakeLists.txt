cmake_minimum_required(VERSION 3.12)

if(FORCPU)
    set(CMAKE_CXX_STANDARD 11)
    find_package(SDL2 REQUIRED)
else(FORCPU)
    add_definitions ( -DFORMPU=1 )
    include(pico_sdk_import.cmake)    
endif(FORCPU)


project(GameTiger VERSION 1.0.0)
 
add_executable(${PROJECT_NAME} 
    core/common.cpp
    core/display.cpp
    core/image.cpp
    core/battery.cpp
    core/keyboard.cpp
    core/tilemap.cpp
    screens/splashscreen.cpp
    screens/menuscreen.cpp
    screens/snakescreen.cpp
    screens/g2048screen.cpp
    screens/tetrisscreen.cpp
    screens/minescreen.cpp
    screens/ticscreen.cpp
    main.cpp
)

if(FORCPU)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
else(FORCPU)
    target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi hardware_pwm hardware_adc hardware_dma hardware_flash hardware_sync)
endif(FORCPU)

if(FORCPU)
else(FORCPU)
    pico_sdk_init()
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 1)

    pico_add_extra_outputs(${PROJECT_NAME})
endif(FORCPU)