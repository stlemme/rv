if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wpedantic")

    option(RV_ENABLE_ASAN "enable the AddressSanitzer" OFF)
    if(RV_ENABLE_ASAN)
        message(STATUS "-- Building with AddressSanitzer (RV_ENABLE_ASAN)")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()

    add_definitions(${LLVM_DEFINITIONS})

    # this defaults to no-rtti builds, if this flag is not available
    if(LLVM_ENABLE_RTTI)
        add_definitions("-frtti")
    else()
        add_definitions("-fno-rtti")
    endif()
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${LLVM_INCLUDE_DIRS})

# libRV
add_subdirectory(rv)

# rvTool
add_subdirectory(tools)

