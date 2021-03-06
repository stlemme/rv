# pre-compilation setup
ADD_DEFINITIONS ( ${LLVM_DEFINITIONS} )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

IF ( ENABLE_ASAN ) 
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
ENDIF()

# this defaults to no-rtti builds, if this flag is not available
IF (LLVM_ENABLE_RTTI)
	ADD_DEFINITIONS( "-frtti" )
ELSE ()
	ADD_DEFINITIONS( "-fno-rtti" )
ENDIF ()

ADD_DEFINITIONS ( "-std=c++14" )

# enable c++11
IF (NOT MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-rtti -Wall" ) 
ELSE ()
  ADD_DEFINITIONS ( "-DRV_LIB" )
  ADD_DEFINITIONS ( "-DRV_STATIC_LIBS" )
    # suppress redundant warnings
    ADD_DEFINITIONS ( "-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS" )
    ADD_DEFINITIONS ( "/wd4244 /wd4800")
ENDIF ()

SET ( RV_GLOBAL_INCLUDES "${PROJ_ROOT_DIR}/include" )
FILE ( GLOB RV_GLOBAL_INCLUDE_FILES "${RV_GLOBAL_INCLUDES}/*.h" )

INCLUDE_DIRECTORIES ( ${PROJ_SOURCE_DIR} )
INCLUDE_DIRECTORIES ( ${RV_GLOBAL_INCLUDES} )
INCLUDE_DIRECTORIES ( ${LLVM_INCLUDE_DIRS} )


# get source files
SET ( RVTOOL_NAME rvTool )

SET ( RVTOOL_SOURCE_FILES rvTool.cpp rvTool.h )
INCLUDE_DIRECTORIES ( include/ )

# ???
INCLUDE ( rv-shared )

# configure LLVM
LINK_DIRECTORIES ( ${LLVM_LIBRARY_DIRS} )
get_rv_llvm_dependency_libs ( LLVM_LIBRARIES )

ADD_EXECUTABLE ( ${RVTOOL_NAME} ${RVTOOL_SOURCE_FILES} )
TARGET_LINK_LIBRARIES ( ${RVTOOL_NAME} ${LLVM_LIBRARIES} ${LIBRARY_NAME} )

# install
INSTALL( TARGETS ${RVTOOL_NAME} RUNTIME DESTINATION bin )
