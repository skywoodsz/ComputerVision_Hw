cmake_minimum_required(VERSION 3.20)
project(Hw6)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Hw6 skin_detector.cpp) #main.cpp skin_detector.cpp
target_link_libraries(Hw6 ${OpenCV_LIBS})
