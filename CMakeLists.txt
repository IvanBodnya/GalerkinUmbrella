cmake_minimum_required(VERSION 3.10)
project(HeatSolver1D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
add_executable(heat_solver src/main.cpp)