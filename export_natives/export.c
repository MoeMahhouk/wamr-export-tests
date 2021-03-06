  #include <stdlib.h>
  #include <stdio.h>
  #include <stdint.h>
  #include "wasm_export.h"


void my_printf(wasm_exec_env_t exec_env, char *text)
{
    printf("%s\n",text);
}

void my_nprintf(wasm_exec_env_t exec_env, char *text, int len)
{
    printf("message: '%.*s'\n", len, text);
}

void hello_world(wasm_exec_env_t exec_env)
{
    printf("simple hello world is here \n");
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

    char *buffer = malloc(fsize + 1);
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
    static NativeSymbol native_symbols[] =
    {
        {
            "my_printf", 		// the name of WASM function name
            my_printf,          // the native function pointer
            "($)",		        // the function prototype signature
            NULL,               // no attachments
        },
        {
            "my_nprintf", 		// the name of WASM function name
            my_nprintf, 		// the native function pointer
            "(*~)",			    // the function prototype signature
            NULL,               // no attachments
        },
        {
            "hello_world", 		// the name of WASM function name
            hello_world, 		// the native function pointer
            "()",			    // the function prototype signature
            NULL,               // no attachments       
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
    module = wasm_runtime_load(buffer, size, error_buf, sizeof(error_buf));
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

    if (wasm_runtime_call_wasm(exec_env, func, 0, args) ) {
      /* the return value is stored in argv[0] */
      //printf("the main function returned %d \n", args[0]);
    }
    else {
      /* exception is thrown if call fails */
      printf("%s\n", wasm_runtime_get_exception(module_inst));
    }
    wasm_runtime_destroy();

    return 0;
}