cmake_minimum_required(VERSION 3.0)

project(fty-service-status)

include(GNUInstallDirs)
include(CTest)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${CMAKE_DL_LIBS})

target_include_directories(${PROJECT_NAME} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

install(TARGETS ${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

if(BUILD_TESTING)
        add_subdirectory(example)
        add_subdirectory(test)
endif()

