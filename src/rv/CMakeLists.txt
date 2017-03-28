# SET ( SLEEF_LIB_PATH "${CMAKE_BINARY_DIR}/rvlib.bc" CACHE FILEPATH "Wfv libary path" )
# SET ( RV_SLEEF_BC_DIR "${PROJ_ROOT_DIR}/sleefsrc" )
# SET ( RV_LIB_MATH_DIR "${RV_LIB_DIR}/mathfun" )

# setup additional lib definition
add_definitions("-DRV_SLEEF_BC_DIR=\"${RV_SLEEF_BC_DIR}\"")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# get source files
set(RV_SOURCE_FILES
    PlatformInfo.cpp
    rvConfig.cpp
    rvConfig.h
    rv.cpp
    sleefLibrary.cpp
    vectorizationInfo.cpp
    VectorizationInfoProxyPass.cpp
    vectorMapping.cpp
    vectorShape.cpp

    analysis/ABAAnalysis.cpp
    analysis/BranchDependenceAnalysis.cpp
    analysis/DFG.cpp
    analysis/MandatoryAnalysis.cpp
    analysis/maskAnalysis.cpp
    analysis/reductionAnalysis.cpp
    analysis/VectorizationAnalysis.cpp

    native/MemoryAccessGrouper.cpp
    native/MemoryAccessGrouper.h
    native/NatBuilder.cpp
    native/NatBuilder.h
    native/nativeBackendPass.cpp
    native/nativeBackendPass.h
    native/ShuffleBuilder.cpp
    native/ShuffleBuilder.h
    native/Utils.cpp
    native/Utils.h

    region/LoopRegion.cpp
    region/Region.cpp

    transform/Linearizer.cpp
    transform/loopExitCanonicalizer.cpp
    transform/maskGenerator.cpp
    transform/structOpt.cpp

    utils/maskGraphUtils.cpp
    utils/rvTools.cpp
    utils/rvTools.h
    utils/stringUtils.h
)

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
