cmake_minimum_required(VERSION 3.28)
project(LTOSC)

set(CMAKE_CXX_STANDARD 20)

add_executable(LTOSC main.cpp)

find_package(asio CONFIG REQUIRED)
target_link_libraries(LTOSC PRIVATE asio::asio)
