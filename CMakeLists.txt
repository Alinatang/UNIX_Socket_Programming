cmake_minimum_required(VERSION 3.15)
project(project)

set(CMAKE_CXX_STANDARD 14)

add_executable(project main.cpp aws.cpp serverA.cpp serverB.cpp client.cpp)