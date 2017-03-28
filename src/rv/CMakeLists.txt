# SET ( SLEEF_LIB_PATH "${CMAKE_BINARY_DIR}/rvlib.bc" CACHE FILEPATH "Wfv libary path" )
SET ( RV_SLEEF_BC_DIR "${PROJ_ROOT_DIR}/sleefsrc" )
# SET ( RV_LIB_MATH_DIR "${RV_LIB_DIR}/mathfun" )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

add_definitions(${LLVM_DEFINITIONS})

# setup additional lib definition
ADD_DEFINITIONS ( "-DRV_SLEEF_BC_DIR=\"${RV_SLEEF_BC_DIR}\"" )

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

# this defaults to no-rtti builds, if this flag is not available
if(LLVM_ENABLE_RTTI)
    add_definitions("-frtti")
else()
    add_definitions("-fno-rtti")
endif()

# enable c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wpedantic" )

# SET ( RV_GLOBAL_INCLUDES "${PROJ_ROOT_DIR}/include" )
# FILE ( GLOB RV_GLOBAL_INCLUDE_FILES "${RV_GLOBAL_INCLUDES}/rv/**/*.h" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# INCLUDE_DIRECTORIES ( ${RV_GLOBAL_INCLUDES} )
include_directories(${LLVM_INCLUDE_DIRS})

# get source files
FILE ( GLOB NATIVE_CPP          native/*.cpp )
FILE ( GLOB CPP_GLOBAL          *.cpp )
FILE ( GLOB CPP_UTILS           utils/*.cpp )
FILE ( GLOB CPP_ANALYSIS        analysis/*.cpp )
FILE ( GLOB REGIONS             region/*.cpp )
FILE ( GLOB TRANSFORMS          transform/*.cpp )
FILE ( GLOB OPT_PASSES pass/*.cpp )

SET ( RV_SOURCE_FILES ${CPP_GLOBAL} ${CPP_UTILS} ${CPP_ANALYSIS} ${REGIONS} ${NATIVE_CPP} ${OPT_PASSES} ${TRANSFORMS} )

# create libRV
add_library(RV ${RV_SOURCE_FILES})

llvm_map_components_to_libnames(LLVM_LIBRARIES analysis irreader support core transformutils)
target_link_libraries(RV ${LLVM_LIBRARIES})

add_dependencies(RV libsleef_x64)

# make install
# SET ( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

# public headers
# FILE ( GLOB RV_INCLUDES_RV "${RV_GLOBAL_INCLUDES}/rv/*.h" )
# INSTALL ( FILES ${RV_INCLUDES_RV} DESTINATION ${PROJ_INCLUDE_DIR}/rv )
# FILE ( GLOB RV_INCLUDES_RV_ANALYSIS "${RV_GLOBAL_INCLUDES}/rv/analysis/*.h" )
# INSTALL ( FILES ${RV_INCLUDES_RV_ANALYSIS} DESTINATION ${PROJ_INCLUDE_DIR}/rv/analysis )
# FILE ( GLOB RV_INCLUDES_RV_REGION "${RV_GLOBAL_INCLUDES}/rv/Region/*.h" )
# INSTALL ( FILES ${RV_INCLUDES_RV_REGION} DESTINATION ${PROJ_INCLUDE_DIR}/rv/Region )
# FILE ( GLOB RV_INCLUDES_RV_TRANSFORMS "${RV_GLOBAL_INCLUDES}/rv/transforms/*.h" )
# INSTALL ( FILES ${RV_INCLUDES_RV_TRANSFORMS} DESTINATION ${PROJ_INCLUDE_DIR}/rv/transforms )
# FILE ( GLOB RV_INCLUDES_RV_UTILS "${RV_GLOBAL_INCLUDES}/rv/utils/*.h" )
# INSTALL ( FILES ${RV_INCLUDES_RV_UTILS} DESTINATION ${PROJ_INCLUDE_DIR}/rv/utils )
# INSTALL ( FILES ${RV_LIB_PATH} DESTINATION ${PROJ_LIBRARY_DIR} )

# libRV
# install(TARGETS RV LIBRARY DESTINATION lib)

# make rvTestSuite
# SET ( RV_TEST_PATH "${PROJ_ROOT_DIR}/test" )

# IF ( APPLE )
    # SET ( STD_LIB "-std=c++11" "-stdlib=libstdc++" )
# ENDIF ()

# INCLUDE_DIRECTORIES ( ${RV_TEST_PATH}/include )

#TODO: clion workaround for install target. remove before release
# add_custom_target(install_${PROJECT_NAME}
        # make install
        # COMMENT "Installing ${PROJECT_NAME}")
