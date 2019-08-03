cmake_minimum_required(VERSION 3.14)
project(sfmlTest)

set(CMAKE_CXX_STANDARD 14)

add_executable(sfmlTest main.cpp)

#need to include library after adding the executable
include_directories("/usr/include/SFML")
target_link_libraries(sfmlTest sfml-graphics sfml-window sfml-audio sfml-system sfml-network)