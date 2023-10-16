<p align="center">
    <img src="docs/tiger.png" width="200">
</p>
<p align="center">
    <img src="docs/gametiger.png" width="300"></p>
</p>

## About GameTiger Console
A Retro style game console designed and built entirely for educational purpose and fun. In the heart of the device is the RP2040 MCU and is built using readily available consumer modules. The whole purpose of the project is to keep everything open source and add more retro games.

![GameTiger Console](/docs/gametiger-gameboy.jpeg)

## Current State
### Snake Game
![Snake Game GameTiger Console](/docs/Snake.gif)

### 2048 Game
![2048 Game GameTiger Console](/docs/2048.gif)

### Tetris Game
![Tetris Game GameTiger Console](/docs/Tetris.gif)

### Minesweeper Game
![Minesweeper Game GameTiger Console](/docs/Minesweeper.gif)

### Tic Tac Toe Game
![Tic Tac Toe Game GameTiger Console](/docs/TicTacToe.gif)

### Ninja Frog Game
![Ninja Frog Game GameTiger Console](/docs/NinjaFrog.gif)

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

[![GameTiger Console](https://img.youtube.com/vi/edIZAm9zVC8/0.jpg)](https://youtu.be/edIZAm9zVC8)

[![GameTiger Console](https://img.youtube.com/vi/e6qAUd7l6A4/0.jpg)](https://youtu.be/e6qAUd7l6A4)


## Build your favourite games
If you wish to contribute to this repo, please feel free to implement your favourite games and send a pull request. 

## To Do
* Add more games
* Design Case in CAD software for 3d printing
* Design PCB for holding all parts together

## License
The entire project is open-sourced software licensed under the MIT license.

## References
* [Hardware Design with RP2040](https://datasheets.raspberrypi.com/rp2040/hardware-design-with-rp2040.pdf)
* [Minimal RP2040 Board](http://www.technoblogy.com/show?3U75)
* [HAGL Graphics Library](https://github.com/tuupola/hagl)
* [PCB Design for RP2040 youtube video](https://www.youtube.com/watch?v=kcwvuwetgEQ)