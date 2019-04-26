if [-d build]; then
    mkdir build
fi
cd build
cmake ..
make
./nes ../roms/cpu_dummy_reads.nes
