cmake_minimum_required (VERSION 2.8...3.16)
project (export_test)

set (WAMR_ROOT_DIR ./wamr) #ToDo better search for the installed wamr library
set (WASI_VERSION 12)
set (WASI_VERSION_FULL ${WASI_VERSION}.0)
set (WASI_SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wasi-sdk)
set (WASI_CLANG ${WASI_SDK_DIR}/bin/clang)
set (WASI_SYSROOT ${WASI_SDK_DIR}/share/wasi-sysroot)
set (WAMR_EMBEDDING_API_INCLUDE_PATH  ${WAMR_ROOT_DIR}/core/iwasm/include)
set (VM_LIB ${WAMR_ROOT_DIR}/product-mini/platforms/linux/build/libvmlib.a)


set(CMAKE_CXX_COMPILER "clang++")

# Reset default linker flags
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")


set(WAMR_BUILD_PLATFORM "linux")
set(WAMR_BUILD_TARGET "X86_64")

set (WAMR_BUILD_INTERP 1)
set (WAMR_BUILD_FAST_INTERP 1)
set (WAMR_BUILD_AOT 0)
set (WAMR_BUILD_JIT 1)
set (WAMR_BUILD_MULTI_MODULE 1)
set (WAMR_BUILD_LIBC_BUILTIN 1)
set (WAMR_BUILD_LIBC_WASI 1)


set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -fPIE")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat -Wformat-security")

include (${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)
add_library(vmlib STATIC ${WAMR_RUNTIME_LIB_SOURCE})

add_custom_target(run ALL
    DEPENDS wasm_apps)

if (NOT EXISTS ${WASI_SDK_DIR})    
    execute_process(
        # Downloading the wasi sdk files
        COMMAND wget https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${WASI_VERSION}/wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_QUIET
        ERROR_QUIET
    )
    execute_process(
         # Creating directory for wasi sdk and extracting the downloaded files
         COMMAND mkdir -p ${WASI_SDK_DIR}
         COMMAND tar -zxvf wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz -C ${WASI_SDK_DIR} --strip 1
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         OUTPUT_QUIET
         ERROR_QUIET
     )
     execute_process(
        # Cleaning up after installation
         COMMAND rm wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         OUTPUT_QUIET
         ERROR_QUIET
     )
endif()

add_custom_command(
    OUTPUT  wasm_apps
    COMMAND make all && mv *.wasm build/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(EXPORT_SOURCE_FILES export.c)

add_executable(export_test ${EXPORT_SOURCE_FILES})
target_include_directories(export_test PUBLIC ${WAMR_EMBEDDING_API_INCLUDE_PATH} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(export_test PRIVATE vmlib pthread m)


set(MULTMOD_SOURCE_FILES multimod.c)
add_executable(multimod_test ${MULTMOD_SOURCE_FILES})
target_include_directories(multimod_test PUBLIC ${WAMR_EMBEDDING_API_INCLUDE_PATH} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(multimod_test PRIVATE vmlib pthread m)