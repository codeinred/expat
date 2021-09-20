# Requirements
cmake_minimum_required(VERSION 3.14)

################################
## Section 1: Declare project ##
################################
set(project_name expat)
set(libname "expatlib")
project(${project_name} CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/helper.cmake")


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

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

find_or_fetch(conduit "https://github.com/codeinred/conduit.git" main)
find_or_fetch(noam "https://github.com/codeinred/noam.git" main)
find_or_fetch(fmt "https://github.com/fmtlib/fmt.git" master)

list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")

set(CMAKE_CXX_FLAGS "-fcoroutines")
find_package(Boost 1.77.0 REQUIRED COMPONENTS system regex)

target_include_directories(${libname} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(
    ${libname}
    INTERFACE
    conduit
    noam
    fmt
    Boost::system
    Boost::regex
    Threads::Threads)

if(PROJECT_IS_TOP_LEVEL)
    set(BENCHMARK_ENABLE_TESTING OFF)
    # find_or_fetch(benchmark "https://github.com/google/benchmark.git" main)

    add_source_dir(src ${libname})
    add_test_dir(test ${libname})
endif()
