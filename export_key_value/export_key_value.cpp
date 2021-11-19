#include <bits/stdint-uintn.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wasm_export.h"
#include <map>
#include <iostream>

#include <chrono>
#include <utility>

typedef std::chrono::high_resolution_clock::time_point TimeVar;

#define duration(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()

std::map<int, char *> int_string_map = {{}};




int get_key_value(wasm_exec_env_t exec_env, int index)
{

    if(!int_string_map.count(index))
    {
        printf("Error(get_key_value): index out of bound \n");
        exit(EXIT_FAILURE);
    }

    wasm_module_inst_t module_inst = get_module_inst(exec_env);


    #if defined VARIANT && VARIANT == 1
    /* Variant 1 */
    return wasm_runtime_addr_native_to_app(module_inst, int_string_map[index]);
    #endif

    #if defined VARIANT && VARIANT == 2
    /* Variant 2 */
    char *native_memory = NULL;
    uint32_t wasm_buffer= 0;
    uint32_t value_size = strlen(int_string_map[index]);
    wasm_buffer = wasm_runtime_module_malloc(module_inst, value_size,(void **)&native_memory);   // should be later free either from host or export wasm_runtime_module_free() as a native function
    memcpy(native_memory, int_string_map[index], value_size);
    return wasm_buffer;
    #endif


    #if defined VARIANT && VARIANT == 3
    /* Variant 3 */
    wasm_function_inst_t func = wasm_runtime_lookup_function(module_inst, "module_malloc", "(i)i");
    uint32_t args[2] = {0};
    args[0] = strlen(int_string_map[index]);
    if (wasm_runtime_call_wasm(exec_env, func, 1, args) ) {
      
        char *buffer = (char *) wasm_runtime_addr_app_to_native(module_inst,args[0]);
        strcpy(buffer, int_string_map[index]);     
        return args[0];

    }   
    else {
      /* exception is thrown if call fails */
      printf("%s\n", wasm_runtime_get_exception(module_inst));
      exit(EXIT_FAILURE);
    }
    #endif
}
void native_module_free(wasm_exec_env_t exec_env, uint32_t ptr) 
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    wasm_runtime_module_free(module_inst, ptr);
}

void set_key_value(wasm_exec_env_t exec_env, int index, char *value)
{
    int_string_map[index] = value;
}



char *read_wasm_binary_to_buffer(const char *path, uint32_t *size)
{
    FILE *fd = fopen(path, "rb");
    if(!fd)
    {
        fprintf(stderr,"failed to open the %s file\n", path);
        perror("fopen failed!\n");
        return NULL;
    }

    if (fseek(fd, 0, SEEK_END) != 0)
    {
        perror("fseek failed\n");
        return NULL;
    }
    long fsize = ftell(fd);
    if(fseek(fd, 0, SEEK_SET) != 0)  // rewind(fd); can be used here too
    {
        perror("fseek failed\n");
        return NULL;
    }

    char *buffer = (char *) malloc(fsize + 1);
    if (!buffer)
    {
        perror("malloc for file buffer failed!\n");
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, fsize, fd);
    if(read_bytes != (size_t)fsize)
    {
        free(buffer);
        fprintf(stderr, "file read didnt read the whole file size and returned only %zu instead of %zu \n", read_bytes, fsize);
        exit(EXIT_FAILURE);
    }
    fclose(fd);
    *size = fsize; //+ 1;
    //buffer[fsize] = 0;
    return buffer;
}



int main(int argc, char *argv[])
{
    int_string_map[0]="test";
    static NativeSymbol native_symbols[] =
    {
        {
            "set_key_value", 		            // the name of WASM function name
            (void *) set_key_value, 		    // the native function pointer
            "(i$)",			                    // the function prototype signature
            NULL,                               // no attachments
        },
        {
            "get_key_value", 		            // the name of WASM function name
            (void *) get_key_value, 		    // the native function pointer
            "(i)i",			                    // the function prototype signature
            NULL,                               // no attachments       
        },
        {
            "native_module_free",               // the name of WASM function name
            (void *) native_module_free,        // the native function pointer
            "(i)",                              // the function prototype signature
            NULL                                // no attachments 
        }
    };

    char *buffer, error_buf[128];
    wasm_module_t module;
    wasm_module_inst_t module_inst;
    wasm_function_inst_t func;
    wasm_exec_env_t exec_env;
    uint32_t size, stack_size = 8092, heap_size = 8092;

    /* initialize the wasm runtime by default configurations */
    if(!wasm_runtime_init())
    {
        fprintf(stderr,"wasm runtime initialisation failed\n");
        exit(EXIT_FAILURE);
    }

    /* read WASM file into a memory buffer */
    buffer = read_wasm_binary_to_buffer(argv[1], &size);
    if(buffer == NULL)
    {
        fprintf(stderr, "the returned buffer for reading the wasm module is null\n");
        exit(EXIT_FAILURE);
    }


    /* add line below if we want to export native functions to WASM app */
    int n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
    if (!wasm_runtime_register_natives("env", native_symbols, n_native_symbols))
    {
        free(buffer);
        exit(EXIT_FAILURE);
    }

    /* parse the WASM file from buffer and create a WASM module */
    module = wasm_runtime_load((uint8_t *)buffer, size, error_buf, sizeof(error_buf));
    if(module == NULL)
    {
        fprintf(stderr, "failed to load the module %s from the buffer with the error %s\n", argv[1], error_buf);
        exit(EXIT_FAILURE);
    }


    /* create an instance of the WASM module (WASM linear memory is ready) */
    module_inst = wasm_runtime_instantiate(module, stack_size, heap_size,
                                            error_buf, sizeof(error_buf));

    if(module_inst == NULL)
    {
        fprintf(stderr, "module instantiation failed!\n");
        exit(EXIT_FAILURE);
    }


      /* lookup a WASM function by its name
     The function signature can NULL here */
    func = wasm_runtime_lookup_function(module_inst, "main", "()i");
    if (func == NULL) {
        printf("multadd function not found\n");
        return -1;
    }

    /* creat an execution environment to execute the WASM functions */
    exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
    if (exec_env == NULL) {
        printf("execution environmnet creation failed\n");
        return -1;
    }
    uint32_t args[1] = {0};
    #ifdef BENCH_ITERATIONS
    TimeVar t1=timeNow();
    #endif

    if (wasm_runtime_call_wasm(exec_env, func, 0, args) ) {
      /* the return value is stored in argv[0] */
      //printf("the main function returned %ld \n", args[0]);
      #ifdef BENCH_ITERATIONS

      auto time_elapsed= duration(timeNow()-t1);
      std::cout<< "total execution time is " << time_elapsed << "ns and " << time_elapsed/BENCH_ITERATIONS << "ns per function call on average" << std::endl; 
      #endif
    }
    else {
      /* exception is thrown if call fails */
      printf("%s\n", wasm_runtime_get_exception(module_inst));
    }
    wasm_runtime_destroy();

    return 0;
}