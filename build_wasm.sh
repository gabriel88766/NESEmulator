#!/bin/bash
rm -rf build_wasm
mkdir build_wasm
cd build_wasm
mkdir includes
cp -r ../includes/* includes
cd includes
emcc -O2 *.cpp -c
cd ..
emcc -O2 --em-config ../config_wasm ../app_wasm.cpp includes/*.o -s USE_SDL=2 -s EXPORTED_FUNCTIONS='["_load_cartridge", "_main"]' -s EXPORTED_RUNTIME_METHODS='["ccall"]' -o emunes.js

#now deploy your .wasm and .js 