# Developer's Guide

This page contains details on setting up the development environment  for writing your own software for GameTiger console

## Building the firmware

First install required packages
```bash
sudo apt-get update
sudo apt-get install  git cmake build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

Clone the main repo
```bash
git clone https://github.com/codetiger/GameTiger-Console
```

Clone *pico-sdk* and *pico-extras* repository, since we will need them for compiling our code

```bash
git clone -b master --recurse-submodules https://github.com/codetiger/pico-sdk
git clone -b master --recurse-submodules https://github.com/raspberrypi/pico-extras
```

Set required environment variables (make sure to use proper paths depending upon your setup)

```bash
export PICO_SDK_PATH=/workspace/pico-sdk
export PICO_TOOLCHAIN_PATH=/usr
export PICO_EXTRAS_PATH=/workspace/pico-extras
```

Now let us compile the software for GameTiger console

```bash
cd GameTiger-Console
cmake .
make
```

On successful completion, you should see something similar to below output:

```bash
[ 97%] Performing build step for 'PioasmBuild'
[ 10%] Building CXX object CMakeFiles/pioasm.dir/main.cpp.o
[ 20%] Building CXX object CMakeFiles/pioasm.dir/pio_assembler.cpp.o
[ 30%] Building CXX object CMakeFiles/pioasm.dir/pio_disassembler.cpp.o
[ 40%] Building CXX object CMakeFiles/pioasm.dir/gen/lexer.cpp.o
[ 50%] Building CXX object CMakeFiles/pioasm.dir/gen/parser.cpp.o
[ 60%] Building CXX object CMakeFiles/pioasm.dir/c_sdk_output.cpp.o
[ 70%] Building CXX object CMakeFiles/pioasm.dir/python_output.cpp.o
[ 80%] Building CXX object CMakeFiles/pioasm.dir/hex_output.cpp.o
[ 90%] Building CXX object CMakeFiles/pioasm.dir/ada_output.cpp.o
[100%] Linking CXX executable pioasm
[100%] Built target pioasm
[ 98%] No install step for 'PioasmBuild'
[100%] Completed 'PioasmBuild'
[100%] Built target PioasmBuild
```

Once the compilation is successful, a firmware file called '*GameTiger.uf2*' should be generated in the '*GameTiger-Console*' folder.
Power on the console by holding the *boot* button and copy the '*GameTiger.uf2*' to the device.