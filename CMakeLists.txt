cmake_minimum_required(VERSION 3.25)
project(MultiThreadedCLion)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-lncurses")

add_executable(MultiThreadedCLion main.cpp RenderJob.cpp SimulateMotionJob.cpp RandomizeJob.cpp SIMDPlatform.h)
