# 6073nes
Cycle-accurate NES emulator

## How to run
In the root directory, run the following commands.
```
cd build
cmake ..
make
./nes [directory to rom]
```

To test out the example, try this.
```
cd build
cmake ..
make
./nes ../roms/cpu_dummy_reads.nes
```

## To-do

### Implement the CPU

Open up `src/cpu.cxx` and implement the `execute()` method.
The `execute()` method executes one instruction, changing the PC as needed.
Use the `mem_read()` method in the memory object to access the CPU memory of the NES's 6502.
