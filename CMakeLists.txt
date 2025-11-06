cmake_minimum_required(VERSION 3.16)
project(emuNES LANGUAGES C CXX)

# Set output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(SOURCES
    includes/6502.cpp
    includes/bus.cpp
    includes/ppu.cpp
    includes/cartridge.cpp
    includes/apu.cpp
    vendored/tinyfiledialogs/tinyfiledialogs.c
)

set(HEADERS
    includes/6502.h
    includes/bus.h
    includes/ppu.h
    includes/cartridge.h
    includes/apu.h
    includes/pallete.h
    vendored/tinyfiledialogs/tinyfiledialogs.h
)

# Add SDL as subdirectory
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)


if(EMSCRIPTEN)
  list(APPEND SOURCES app_wasm.cpp)
  set(CMAKE_EXECUTABLE_SUFFIX ".js" CACHE INTERNAL "")
else()
  list(APPEND SOURCES app.cpp)
endif()

# Build executable
add_executable(emuNES ${SOURCES} ${HEADERS})

if(EMSCRIPTEN)
  target_link_options(emuNES PRIVATE -sEXPORTED_FUNCTIONS=['_load_cartridge','_main'] -sEXPORTED_RUNTIME_METHODS=['ccall'])
endif()

# Link to SDL2
target_link_libraries(emuNES PRIVATE SDL3::SDL3 )

# Set C++ standard
target_compile_features(emuNES PRIVATE cxx_std_17)

#Optimize
target_compile_options(emuNES PRIVATE -O2)