# Sleef lib
# SET ( SLEEF_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Path to libsleef sources" FORCE )
# SET ( RV_LIB_SLEEF_DIR "${SLEEF_DIR}/simd" )
# SET ( RV_LIB_SLEEF_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build )

set(SLEEF_BC_AVX2_SP avx2_sleef_sp.bc)
set(SLEEF_BC_AVX_SP avx_sleef_sp.bc)
set(SLEEF_BC_SSE_SP sse_sleef_sp.bc)

set(SLEEF_BC_AVX2_DP avx2_sleef_dp.bc)
set(SLEEF_BC_AVX_DP avx_sleef_dp.bc)
set(SLEEF_BC_SSE_DP sse_sleef_dp.bc)

# SET ( RV_LIB_SLEEF_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sleefsrc" )

set(SLEEF_BUILD_FLAGS -emit-llvm -c -Wall -Wno-unused-function -O3)
set(SLEEF_SIMD_SP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/simd/sleefsimdsp.c)
set(SLEEF_SIMD_DP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/simd/sleefsimddp.c)

add_custom_command(OUTPUT ${SLEEF_BC_AVX2_SP}
    COMMAND ${LLVM_TOOL_CLANG} ${SLEEF_SIMD_SP_FILE} ${SLEEF_BUILD_FLAGS} -march=haswell -mavx2 -mfma -DENABLE_AVX2=ON -o ${SLEEF_BC_AVX2_SP}
)
add_custom_command(OUTPUT ${SLEEF_BC_AVX_SP}
    COMMAND ${LLVM_TOOL_CLANG} ${SLEEF_SIMD_SP_FILE} ${SLEEF_BUILD_FLAGS} -mavx -DENABLE_AVX=ON -o ${SLEEF_BC_AVX_SP}
)
add_custom_command(OUTPUT ${SLEEF_BC_SSE_SP}
    COMMAND ${LLVM_TOOL_CLANG} ${SLEEF_SIMD_SP_FILE} ${SLEEF_BUILD_FLAGS} -m64 -msse4 -DENABLE_SSE2=ON -o ${SLEEF_BC_SSE_SP}
)
add_custom_command(OUTPUT ${SLEEF_BC_AVX2_DP}
    COMMAND ${LLVM_TOOL_CLANG} ${SLEEF_SIMD_DP_FILE} ${SLEEF_BUILD_FLAGS} -march=haswell -mavx2 -mfma -DENABLE_AVX2=ON -o ${SLEEF_BC_AVX2_DP}
)
add_custom_command(OUTPUT ${SLEEF_BC_AVX_DP}
    COMMAND ${LLVM_TOOL_CLANG} ${SLEEF_SIMD_DP_FILE} ${SLEEF_BUILD_FLAGS} -mavx -DENABLE_AVX=ON -o ${SLEEF_BC_AVX_DP}
)
add_custom_command(OUTPUT ${SLEEF_BC_SSE_DP}
    COMMAND ${LLVM_TOOL_CLANG} ${SLEEF_SIMD_DP_FILE} ${SLEEF_BUILD_FLAGS} -m64 -msse4 -DENABLE_SSE2=ON -o ${SLEEF_BC_SSE_DP}
)

add_custom_target(libsleef_x64 DEPENDS ${SLEEF_BC_AVX2_SP} ${SLEEF_BC_AVX_SP} ${SLEEF_BC_SSE_SP} ${SLEEF_BC_AVX2_DP} ${SLEEF_BC_AVX_DP} ${SLEEF_BC_SSE_DP})

set(RV_SLEEF_BC_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "path to generated .bc files of sleef")
