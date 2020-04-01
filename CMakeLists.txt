cmake_minimum_required(VERSION 3.0)

project(fty-service-status)

include(GNUInstallDirs)
include(CTest)

#options and default value
option(BUILD_DOC "Build documentation" ON)
option(BUILD_TESTING "Build tests" ON)

#create the header only library
add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${CMAKE_DL_LIBS})

target_include_directories(${PROJECT_NAME} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

target_compile_options(${PROJECT_NAME} INTERFACE 
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
)

#Setup instructions for install
install(TARGETS ${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

#if build tests
if(BUILD_TESTING)
        #build example and test
        add_subdirectory(example)
        add_subdirectory(test)
endif()

#if build documentation
if(BUILD_DOC)
        #create documentation with doxygen
        find_package(Doxygen)
        if (DOXYGEN_FOUND)
        add_custom_target( doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} doxygen.conf
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Doxygen")
        else (DOXYGEN_FOUND)
                message("Doxygen need to be installed to generate the doxygen documentation")
        endif (DOXYGEN_FOUND)
endif()
