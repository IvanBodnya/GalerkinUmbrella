cmake_minimum_required(VERSION 3.15)
project(ThermaGrid VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------------------------
# Core library
# ----------------------------------------------------------------------------

add_library(ThermaGrid_Core
    src/core/QuadTreeNode.cpp
)

target_include_directories(ThermaGrid_Core
    PUBLIC
        src/core
)

# ----------------------------------------------------------------------------
# Main executable (for testing)
# ----------------------------------------------------------------------------

add_executable(ThermaGrid_Main
    src/core/main.cpp
)

target_link_libraries(ThermaGrid_Main
    ThermaGrid_Core
)