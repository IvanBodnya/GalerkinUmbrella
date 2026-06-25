cmake_minimum_required(VERSION 3.14)
project(HeatSolver1D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
include_directories(${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})

# Create executable
add_executable(heat_solver src/main.cpp)

# link python and numpy
target_link_libraries(heat_solver PRIVATE ${Python3_LIBRARIES} Python3::Python Python3::NumPy)