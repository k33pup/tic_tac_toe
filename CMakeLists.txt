cmake_minimum_required(VERSION 3.28.1)
project(tic_tac_toe)

set(CMAKE_CXX_STANDARD 23)

add_executable(server server/server.cpp include/game.cpp)
add_executable(client client/client.cpp)

include_directories(include)