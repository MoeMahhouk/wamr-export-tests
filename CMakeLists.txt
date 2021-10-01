cmake_minimum_required (VERSION 2.8...3.16)
project (export_test)

set (WAMR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}../../wasm-micro-runtime) #ToDo better search for the installed wamr library
set (WASI_SDK_DIR /opt/wasi-sdk-12.0)
set (WASI_CLANG ${WASI_SDK_DIR}/bin/clang)
set (WASI_SYSROOT ${WASI_SDK_DIR}/share/wasi-sysroot)
set (WAMR_EMBEDDING_API_INCLUDE_PATH  ${WAMR_ROOT_DIR}/core/iwasm/include)
set (VM_LIB ${WAMR_ROOT_DIR}/product-mini/platforms/linux/build/vmlib)

#set(CMAKE_CXX_COMPILER "clang++")

# Reset default linker flags
#set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
#set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")


# Set WAMR_BUILD_TARGET, currently values supported:
# "X86_64", "AMD_64", "X86_32", "AARCH64[sub]", "ARM[sub]", "THUMB[sub]",
# "MIPS", "XTENSA", "RISCV64[sub]", "RISCV32[sub]"
#if (NOT DEFINED WAMR_BUILD_TARGET)
#  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
#    set (WAMR_BUILD_TARGET "AARCH64")
#  elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "riscv64")
#    set (WAMR_BUILD_TARGET "RISCV64")
#  elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
#    # Build as X86_64 by default in 64-bit platform
#    set (WAMR_BUILD_TARGET "X86_64")
#  else ()
#    # Build as X86_32 by default in 32-bit platform
#    set (WAMR_BUILD_TARGET "X86_32")
#  endif ()
#endif ()

#if (NOT CMAKE_BUILD_TYPE)
#  set (CMAKE_BUILD_TYPE Release)
#endif ()

#set (WAMR_BUILD_INTERP 1)
#set (WAMR_BUILD_AOT 1)
#set (WAMR_BUILD_JIT 0)
#set (WAMR_BUILD_LIBC_BUILTIN 1)
#
#if (NOT MSVC)
#  set (WAMR_BUILD_LIBC_WASI 1)
#endif ()

#if (NOT MSVC)
  # linker flags
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -fPIE")
#  if (NOT (CMAKE_C_COMPILER MATCHES ".*clang.*" OR CMAKE_C_COMPILER_ID MATCHES ".*Clang"))
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
#  endif ()
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat -Wformat-security")
#  if (WAMR_BUILD_TARGET MATCHES "X86_.*" OR WAMR_BUILD_TARGET STREQUAL "AMD_64")
#    if (NOT (CMAKE_C_COMPILER MATCHES ".*clang.*" OR CMAKE_C_COMPILER_ID MATCHES ".*Clang"))
#      set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mindirect-branch-register")
#    endif ()
#  endif ()
#endif ()

add_custom_target(run ALL
    DEPENDS test.wasm)

add_custom_command(
    OUTPUT test.wasm
    COMMAND ${WASI_CLANG} --sysroot=${WASI_SYSROOT} -Wl,--no-entry -O3 -o test.wasm ${CMAKE_CURRENT_SOURCE_DIR}/test.c
    COMMENT "Compiling the test.c to test.wasm" 
    WORKING_DIRECTORY ${CMAKE_CURRENT_BIN_DIR}
)

#add_executable(test.wasm test.c)
#target_compile_options(test.wasm -sysroot=${WASI_SYSROOT} -Wl,--no-entry -O3)
###### There is no way to specify another compiler for a single target

#include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
#set(C_SOURCE_FILES main.c)

#add_executable(export_test ${C_SOURCE_FILES})
#target_include_directories(export_test ${WAMR_EMBEDDING_API_INCLUDE_PATH})
#target_link_libraries(multi_module PRIVATE ${VM_LIB} -lpthread -lm)