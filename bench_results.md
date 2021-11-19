Number of iterations is 10 000 000 
The function call is a getter function returning a string stored at a specific index in a std::map located on the host (where the embedded runtime is)

| Variant#  | Variant Explanation                                          | Total Runtime(ns) | Avg per call(ns) |
| --------- | ------------------------------------------------------------ | ----------------- | ---------------- |
| Variant 1 | translating the hosts address to the module space using `wasm_runtime_addr_native_to_app` | 2011840921        | 201              |
| Variant 2 | initializing in-module memory from the host using `wamr_runtime_module_malloc` | 5556701863        | 555              |
| Variant 3 | exporting in-module malloc to the host                       | 11245922744       | 1124             |
| Variant 4 | pre-initialized in-module shared memory                      | 2242094792        | 224ns            |

