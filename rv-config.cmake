SET ( PROJ_NAME RV )

FIND_PACKAGE(LLVM 3.8 REQUIRED)

IF( NOT LLVM_FOUND )
  MESSAGE(FATAL_ERROR "LLVM 3.8 not found.")
ENDIF( NOT LLVM_FOUND )

FIND_PATH ( RV_ROOT_DIR rv-config.cmake PATHS ${RV_DIR} $ENV{RV_DIR} $ENV{RV_ROOT} )
SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${RV_ROOT_DIR} )

SET ( RV_OUTPUT_LIBS RV.lib RV.so RV.dll libRV libRV.so libRV.a libRV.dylib libRV.dll )

FIND_PATH ( RV_INCLUDE_DIR NAMES rv/rv.h PATHS ${RV_ROOT_DIR}/include ${RV_ROOT_DIR}/build/include )
FIND_PATH ( RV_LIBRARY_DIR NAMES ${RV_OUTPUT_LIBS} PATHS ${RV_ROOT_DIR}/lib ${RV_ROOT_DIR}/build/lib )

IF ( RV_LIBRARY_DIR )
    FIND_LIBRARY ( RV_LIBRARIES NAMES ${RV_OUTPUT_LIBS} PATHS ${RV_LIBRARY_DIR} )
ENDIF ( RV_LIBRARY_DIR )

IF ( RV_INCLUDE_DIR AND RV_LIBRARIES )
    SET ( RV_FOUND TRUE CACHE BOOL "" FORCE )
ELSE ( RV_INCLUDE_DIR AND RV_LIBRARIES )
    SET ( RV_FOUND FALSE CACHE BOOL "" FORCE )
ENDIF ( RV_INCLUDE_DIR AND RV_LIBRARIES )

INCLUDE ( ${RV_DIR}/rv-shared.cmake )

MARK_AS_ADVANCED ( RV_FOUND )

