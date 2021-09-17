# Requirements
cmake_minimum_required(VERSION 3.14)

################################
## Section 1: Declare project ##
################################
set(project_name expat)
set(libname "expatlib")
project(${project_name} CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/helper.cmake")

###################################
## Section 3: Project CMake code ##
###################################

# Add library and register compile options
add_library(${libname} INTERFACE)

target_cpp_20(${libname})

target_include_directories(
    ${libname}
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include>
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)



if(PROJECT_IS_TOP_LEVEL)
    set(BENCHMARK_ENABLE_TESTING OFF)
    find_or_fetch(noam "https://github.com/codeinred/noam.git" main)
    find_or_fetch(fmt "https://github.com/fmtlib/fmt.git" master)
    # find_or_fetch(benchmark "https://github.com/google/benchmark.git" main)

    add_source_dir(src ${libname} fmt noam)
endif()
