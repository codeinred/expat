# Requirements
cmake_minimum_required(VERSION 3.14)

################################
## Section 1: Declare project ##
################################
set(project_name expat)
set(libname "expatlib")
project(${project_name} CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/helper.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

###################################
## Section 3: Project CMake code ##
###################################

# Add library and register compile options
add_library(${libname} INTERFACE)

target_cpp_20(${libname})

target_include_directories(
    ${libname}
    INTERFACE
    include/)

find_package(Threads REQUIRED)
find_or_fetch(noam "https://github.com/codeinred/noam.git" main)
find_or_fetch(fmt "https://github.com/fmtlib/fmt.git" master)

note("Remote dependencies: ${remote_dependencies}")
FetchContent_MakeAvailable(${remote_dependencies})

target_link_libraries(
    ${libname}
    INTERFACE
    noam
    fmt
    Threads::Threads)

if(PROJECT_IS_TOP_LEVEL)
    set(BENCHMARK_ENABLE_TESTING OFF)
    # find_or_fetch(benchmark "https://github.com/google/benchmark.git" main)

    add_source_dir(src ${libname})
    include(CTest)
    add_test_dir(test ${libname})
endif()
