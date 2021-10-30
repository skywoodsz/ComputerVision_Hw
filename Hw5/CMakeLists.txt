cmake_minimum_required(VERSION 3.20)
project(Hw5)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(Hw5 main.cpp)
target_link_libraries(Hw5 ${OpenCV_LIBS})
