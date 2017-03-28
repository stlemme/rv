
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

add_executable(rvTool rvTool.cpp rvTool.h ArgumentReader.h)

llvm_map_components_to_libnames(LLVM_LIBRARIES analysis irreader support core transformutils)
target_link_libraries(rvTool ${LLVM_LIBRARIES} RV)

# install
#INSTALL( TARGETS ${RVTOOL_NAME} RUNTIME DESTINATION bin )
