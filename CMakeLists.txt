cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(RV)

set(PACKAGE_VERSION "2.0")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited config" FORCE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug or Release" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(LLVM 3.8 CONFIG REQUIRED)

# Sleef
option(RV_ENABLE_SLEEF "enable the usage of sleef in libRV" ON)
if(RV_ENABLE_SLEEF)
    add_subdirectory(sleef)
endif()

# TODO: move header files to src and export public headers via install
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# libRV
add_subdirectory(src)

# rvTool
#add_subdirectory(tools)

export(TARGETS rvTool RV FILE ${CMAKE_BINARY_DIR}/share/RV/cmake/rv-exports.cmake)
# configure_file(cmake/impala-config.cmake.in ${CMAKE_BINARY_DIR}/share/impala/cmake/impala-config.cmake @ONLY)

# if (BUILD_TESTING)
    # enable_testing()
    # add_subdirectory(test)
# endif(BUILD_TESTING)
