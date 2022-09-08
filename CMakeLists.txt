cmake_minimum_required(VERSION 3.16.3)

# set the project name
project(NESEmulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(gvNES.out main.cpp includes/6502.h includes/6502.cpp)


add_subdirectory(includes)




target_include_directories(includes PUBLIC ${PROJECT_SOURCE_DIR}/includes)
