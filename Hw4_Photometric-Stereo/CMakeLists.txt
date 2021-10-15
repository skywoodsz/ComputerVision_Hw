cmake_minimum_required(VERSION 3.20)
project(Hw4)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)
FIND_PACKAGE(VTK REQUIRED)

INCLUDE(${VTK_USE_FILE})
ADD_DEFINITIONS(-std=c++11)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("/usr/include/eigen3")

#add_executable(Hw4 main.cpp test.cpp)
add_executable(Hw4 main.cpp )
target_link_libraries(Hw4 ${OpenCV_LIBS} ${VTK_LIBRARIES})
