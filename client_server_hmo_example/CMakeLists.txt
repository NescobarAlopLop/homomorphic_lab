## Licensed under the MIT license.

cmake_minimum_required(VERSION 3.12)

project(lab VERSION 0.0.1 LANGUAGES CXX)

find_package(SEAL 3.5 REQUIRED)

add_executable(basic_server_client_example ../client_server_hmo_example/basic_example.cpp)

target_link_libraries(basic_server_client_example SEAL::seal)
