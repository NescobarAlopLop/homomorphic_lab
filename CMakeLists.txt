cmake_minimum_required(VERSION 3.16)
project(homomorhpic_lab)

set(CMAKE_CXX_STANDARD 17)

find_package(SEAL 3.7.2 REQUIRED)

include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)

set(source_files
        ${PROJECT_SOURCE_DIR}/src/basic_example.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(client_read_parameters_encrypt_query
        ${PROJECT_SOURCE_DIR}/src/client_read_parameters_encrypt_query.cpp
#        ${PROJECT_SOURCE_DIR}/src/json.hpp
)
target_link_libraries(client_read_parameters_encrypt_query SEAL::seal nlohmann_json::nlohmann_json)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(client_read_results_decrypt_query_result ${PROJECT_SOURCE_DIR}/src/client_read_results_decrypt_query_result.cpp)
target_link_libraries(client_read_results_decrypt_query_result SEAL::seal nlohmann_json::nlohmann_json)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(server_generate_parameters
        ${PROJECT_SOURCE_DIR}/src/server_generate_parameters.cpp
#        ${PROJECT_SOURCE_DIR}/src/json.hpp
)
target_link_libraries(server_generate_parameters SEAL::seal nlohmann_json::nlohmann_json)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(server_run_computation
        ${PROJECT_SOURCE_DIR}/src/server_run_computation.cpp
#        ${PROJECT_SOURCE_DIR}/src/json.hpp
)
target_link_libraries(server_run_computation SEAL::seal nlohmann_json::nlohmann_json)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(basic_server_client_example
        ${PROJECT_SOURCE_DIR}/src/basic_example.cpp
#        ${PROJECT_SOURCE_DIR}/src/json.hpp
)
target_link_libraries(basic_server_client_example SEAL::seal nlohmann_json::nlohmann_json)
