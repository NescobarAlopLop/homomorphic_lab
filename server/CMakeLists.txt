## Licensed under the MIT license.
#
cmake_minimum_required(VERSION 3.12)
#
project(lab VERSION 0.0.1 LANGUAGES CXX)

find_package(SEAL 3.5 REQUIRED)

add_executable(predict main.cpp
        main.h
        predict.cpp
        )

target_link_libraries(predict SEAL::seal)
