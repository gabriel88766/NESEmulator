cmake_minimum_required(VERSION 3.16)
project(emuNES LANGUAGES CXX)

# Set output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


set(SOURCES
    app.cpp
    includes/6502.cpp
    includes/bus.cpp
    includes/ppu.cpp
    includes/cartridge.cpp
    includes/apu.cpp
    vendored/imgui/imgui.cpp
    vendored/imgui/imgui_draw.cpp
    vendored/imgui/imgui_widgets.cpp
    vendored/imgui/imgui_tables.cpp
    vendored/imgui/backends/imgui_impl_sdl2.cpp
    vendored/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

set(HEADERS
    includes/6502.h
    includes/bus.h
    includes/ppu.h
    includes/cartridge.h
    includes/apu.h
    vendored/imgui/imgui.h
    vendored/imgui/imgui_internal.h
    vendored/imgui/backends/imgui_impl_sdl2.h
    vendored/imgui/backends/imgui_impl_sdlrenderer2.h
)

# Add SDL as subdirectory
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# Build our hello executable
add_executable(emuNES ${SOURCES} ${HEADERS})


target_include_directories(emuNES PRIVATE
    vendored/imgui
    vendored/imgui/backends
)

# Link to SDL2
target_link_libraries(emuNES PRIVATE SDL2::SDL2main SDL2::SDL2 )

# Set C++ standard
target_compile_features(emuNES PRIVATE cxx_std_17)