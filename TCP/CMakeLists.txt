cmake_minimum_required(VERSION 3.15)
project(TCP)

set(CMAKE_CXX_STANDARD 14)

add_executable(TCP main.cpp)
add_executable(Server server.h server.cpp)
add_executable(Client client.h client.cpp)