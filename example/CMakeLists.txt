cmake_minimum_required(VERSION 3.0)

project(fty-service-status-example)

add_library(${PROJECT_NAME} SHARED src/example.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)