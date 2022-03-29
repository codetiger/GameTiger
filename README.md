<p align="center">
    <img src="docs/tiger.png" width="200">
</p>
<p align="center">
    <img src="docs/gametiger.png" width="300"></p>
</p>

## About GameTiger Console

A Retro style game console designed and built entirely for educational purpose and fun. In the heart of the device is the RP2040 MCU and is built using readily available consumer modules. The whole purpose of the project is to keep everything open source and add more retro games.


![GameTiger Console](/docs/gametiger.jpg)


## Component specification

*  MCU RP2040
    *  32-bit dual ARM Cortex-M0+ Microcontroller
    *  133 MHz Clock speed
    *  264 KB SRAM
    *  2 MB flash storage
    *  26 GPIO pins
*  LCD display module by Waveshare
    *  Resolution: 240×320
    *  Color: 65K RGB (16bit RGB565)
    *  Interface: SPI
    *  Driver: ST7789
    *  Backlight: LED
    *  Operating voltage: 3.3V/5V
*  Tactile Buttons
*  LiPo SHIM for Pico by Pimoroni
    *  MCP73831 charger
    *  XB6096I2S battery protector
    *  Supports battery level measuring on VSYS pin
*  Witty Fox Li-Ion Battery
    *  Voltage: 3.7v
    *  Capacity: 1000 mAh


## Wiring 

The components are based on standard interfaces and thus nothing complicated in wiring. You can feel free to use different GPIO pins based on lot of tutorials but this is what I've used and configured in the software as default. 

| Component | Pin | Pico GPIO | Description |
|--|--|--|--|
|LCD|VCC|VSYS|Power Input|
||GND|GND|Ground|
||DIN|GP11|MOSI pin of SPI, data transmitted|
||CLK|GP10|SCK pin of SPI, clock pin|
||CS|GP9|Chip selection of SPI, low active|
||DC|GP8|Data/Command control pin (High:data; Low: command)|
||RST|GP12|Reset pin, low active|
||BL|GP13|Backlight control|
|Buttons|Up|GPIO2|Up button in the keypad|
||Down|GPIO0|Down button in the keypad|
||Left|GPIO1|Left button in the keypad|
||Right|GPIO3|Right button in the keypad|
||A|GPIO4|A (Action) button in the keypad|
||B|GPIO5|B (Back) button in the keypad|
|LiPo SHIM| | | Directly mounted on Pico based on datasheet|
____

## Software
The software is build using Pico C++ SDK and has the basic hardware interfacing functionalities implemented. For an example, the popular Snake game is available. The below modules are ready available and is constantly under development. 

* Operating System Drivers
    * Display driver
    * Button interrupts
    * Battery management system driver
* Framebuffer Library
    * Supports transparency
    * Streaming to display memory using DMA
    * Draw primitives (Line, Rect, Fill Rect)
    * Supports drawing images with alpha channel
* Sprite sheet 
    * Support for sprite sheet 
    * Basic tilemap support
* Font system based on Sprite sheet
* Menu system
    * Games
    * Hardware config
        * Display brightness
        * Display sleep time after inactivity

The software is well optimised to achieve a target of 30 frames per second. The Snake game achieves more than 44 FPS on a default settings without overclocking. 

## Current State

### Snake Game
![Snake Game GameTiger Console](/docs/snake.png)

### 2048 Game
![2048 Game GameTiger Console](/docs/2048.png)

### Tetris Game
![Tetris Game GameTiger Console](/docs/tetris.png)

### Minesweeper Game
![Minesweeper Game GameTiger Console](/docs/minesweeper.png)

[![GameTiger Console](https://img.youtube.com/vi/edIZAm9zVC8/0.jpg)](https://youtu.be/edIZAm9zVC8)


## Build your favourite games
If you wish to contribute to this repo, please feel free to implement your favourite games and send a pull request. 

## To Do
* Add more games
* Design Case in CAD software for 3d printing
* Design PCB for holding all parts together


## License
The entire project is open-sourced software licensed under the MIT license.