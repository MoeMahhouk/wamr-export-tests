cmake_minimum_required (VERSION 2.8...3.16)
project (export_test)

set (WAMR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wamr) 
set (WASI_VERSION 12)
set (WASI_VERSION_FULL ${WASI_VERSION}.0)
set (WASI_SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wasi-sdk)
set (WASI_CLANG ${WASI_SDK_DIR}/bin/clang)
set (WASI_SYSROOT ${WASI_SDK_DIR}/share/wasi-sysroot)
set (WAMR_EMBEDDING_API_INCLUDE_PATH  ${WAMR_ROOT_DIR}/core/iwasm/include)
#set (VM_LIB ${WAMR_ROOT_DIR}/product-mini/platforms/linux/build/libvmlib.a)


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

if(NOT BENCH_ITERATIONS)
    SET(BENCH_ITERATIONS 1000000)
endif()
if(NOT VARIANT)
    SET(VARIANT 1)
endif()


add_custom_command(
    OUTPUT  wasm_apps
    COMMAND cd wasm_apps && BENCH_ITERATIONS=${BENCH_ITERATIONS} VARIANT=${VARIANT} make all && mv *.wasm ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


file(GLOB WASM_FILES
        "${CMAKE_BINARY_DIR}/add.wasm"
        "${CMAKE_BINARY_DIR}/multadd.wasm"
        "${CMAKE_BINARY_DIR}/test.wasm"
        "${CMAKE_BINARY_DIR}/key_value.wasm"
        "${CMAKE_BINARY_DIR}/key_value_bench.wasm"
        "${CMAKE_BINARY_DIR}/kv_shared_mem.wasm"

)

set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        "${WASM_FILES}"
)

     
set(EXPORT_SOURCE_FILES export_natives/export.c)
add_executable(export_test ${EXPORT_SOURCE_FILES})
target_include_directories(export_test PUBLIC ${WAMR_EMBEDDING_API_INCLUDE_PATH} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(export_test PRIVATE vmlib pthread m)


set(MULTMOD_SOURCE_FILES multi_modules/multimod.c)
add_executable(multimod_test ${MULTMOD_SOURCE_FILES})
target_include_directories(multimod_test PUBLIC ${WAMR_EMBEDDING_API_INCLUDE_PATH} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(multimod_test PRIVATE vmlib pthread m)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")

set(EXPORT_KEY_VALUE_SOURCE_FILES export_key_value/export_key_value.cpp)
add_executable(export_key_value ${EXPORT_KEY_VALUE_SOURCE_FILES})
target_include_directories(export_key_value PUBLIC ${WAMR_EMBEDDING_API_INCLUDE_PATH} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(export_key_value PRIVATE vmlib pthread m)
target_compile_definitions(export_key_value PUBLIC -DBENCH_ITERATIONS=${BENCH_ITERATIONS} -DVARIANT=${VARIANT})


set(EXPORT_KEY_VALUE_SHARED_MEM_SOURCE_FILES export_key_value_shared_mem/export_key_value_shared_mem.cpp)
add_executable(export_key_value_shared_mem ${EXPORT_KEY_VALUE_SHARED_MEM_SOURCE_FILES})
target_include_directories(export_key_value_shared_mem PUBLIC ${WAMR_EMBEDDING_API_INCLUDE_PATH} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(export_key_value_shared_mem PRIVATE vmlib pthread m)
target_compile_definitions(export_key_value_shared_mem PUBLIC -DBENCH_ITERATIONS=${BENCH_ITERATIONS} -DVARIANT=${VARIANT})


#if(BENCH_ITERATIONS)
#    target_compile_definitions(export_key_value PUBLIC -DBENCH_ITERATIONS=${BENCH_ITERATIONS})
#    target_compile_definitions(export_key_value_shared_mem PUBLIC -DBENCH_ITERATIONS=${BENCH_ITERATIONS})
#else()
#    target_compile_definitions(export_key_value PUBLIC -DBENCH_ITERATIONS=${DEFAULT_BENCHMARK_ITERATIONS})
#    target_compile_definitions(export_key_value_shared_mem PUBLIC -DBENCH_ITERATIONS=${DEFAULT_BENCHMARK_ITERATIONS})
#endif()
#
#if(VARIANT)
#    target_compile_definitions(export_key_value PUBLIC -DVARIANT=${VARIANT})
#    target_compile_definitions(export_key_value_shared_mem PUBLIC -DVARIANT=${VARIANT})
#else()
#    target_compile_definitions(export_key_value PUBLIC -DVARIANT=${DEFAULT_VARIANT})
#    target_compile_definitions(export_key_value_shared_mem PUBLIC -DVARIANT=${DEFAULT_VARIANT})
#endif()