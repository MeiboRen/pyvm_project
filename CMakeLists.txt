cmake_minimum_required(VERSION 4.2)
project(pyvm)

include_directories(include/)

set(sourcecode
src/vm/parser.cpp
src/vm/executor.cpp
src/extern_objs/base_obj.cpp
src/extern_objs/base_class.cpp
src/builtin_objs/inner_array.cpp
src/builtin_objs/inner_dict.cpp
src/extern_objs/data_types/obj_int.cpp
src/extern_objs/data_types/obj_float.cpp
src/extern_objs/data_types/obj_str.cpp
src/extern_objs/data_structs/struct_list.cpp
src/extern_objs/data_structs/struct_dict.cpp
src/extern_objs/ctrl_structs/struct_call.cpp
src/extern_objs/ctrl_structs/struct_closure.cpp
src/extern_objs/ctrl_structs/struct_code.cpp
src/extern_objs/ctrl_structs/struct_func.cpp
src/extern_objs/ctrl_structs/struct_pkg.cpp
src/extern_objs/ctrl_structs/trace.cpp
)

add_executable(pyvm pyvm/pyvm.cpp)
add_library(basevm SHARED ${sourcecode})
target_link_libraries(basevm PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(pyvm PRIVATE basevm)
set_target_properties(basevm PROPERTIES PREFIX "lib")

file(WRITE ${CMAKE_BINARY_DIR}/lib.sh
"#!/bin/bash

for file in lib/__pycache__/*.cpython-*.pyc; do
    if [ -f \"\$file\" ]; then
        base=\$(basename \"\$file\" | sed 's/\\.cpython-[0-9]*//')
        cp \"\$file\" \"lib/\${base}\"
    fi
done

rm -rf lib/__pycache__
rm -f lib/*.py
")

add_custom_target(pylibs ALL
COMMAND chmod +x ${CMAKE_BINARY_DIR}/lib.sh
COMMAND mkdir -p lib
COMMAND cp ../pyvm/builtin.py lib/
COMMAND python3 -m compileall lib/*.py
COMMAND ${CMAKE_BINARY_DIR}/lib.sh
DEPENDS basevm
)