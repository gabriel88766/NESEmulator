cmake_minimum_required(VERSION 3.16)
project(emuNES LANGUAGES CXX)

# Set output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Optional: Force static SDL build (no .so)
set(SDL3_DISABLE_SHARED ON CACHE BOOL "" FORCE)
set(SDL3_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

# Add SDL as subdirectory
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# Build our hello executable
add_executable(emuNES app.cpp)

# Link to SDL3
target_link_libraries(emuNES PRIVATE SDL3::SDL3)

# Set C++ standard
target_compile_features(emuNES PRIVATE cxx_std_17)