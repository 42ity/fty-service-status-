cmake_minimum_required(VERSION 3.0)

project(fty-service-status-test)

#add Catch2
find_package(Catch2 REQUIRED)

include(Catch)

add_executable(${PROJECT_NAME} src/test.cpp)

if(CMAKE_VERSION VERSION_LESS "3.1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

  target_link_libraries(${PROJECT_NAME}
    fty-service-status
    #Catch2::Catch2 => when we will have cmake 3.1
  )

  add_test(NAME ${PROJECT_NAME} COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
else ()
  target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

  target_link_libraries(${PROJECT_NAME}
    fty-service-status
    #Catch2::Catch2 => when we will have cmake 3.1
  )

  catch_discover_tests(${PROJECT_NAME})
endif()

target_compile_options(${PROJECT_NAME} PUBLIC
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
)
