cmake_minimum_required(VERSION 3.0)

project(fty-service-status-test)

add_executable(${PROJECT_NAME} src/test.cpp)
target_link_libraries(${PROJECT_NAME} fty-service-status)

add_test(NAME ${PROJECT_NAME} COMMAND $<TARGET_FILE:${PROJECT_NAME}>)