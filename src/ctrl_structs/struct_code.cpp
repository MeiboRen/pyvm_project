#include "ctrl_structs/struct_code.hpp"
#include "extern_objs/data_types/obj_str.hpp"

class_code *class_code::instance = NULL;

class_code::class_code() {
    set_name(new obj_str("code"));
    add_super_list(class_obj::return_instance());
    set_type_object(new obj_type());
}

class_code *class_code::return_instance() {
    if (instance == NULL) instance = new class_code();
    return instance;
}

size_t class_code::size() {
    return sizeof(struct_code);
}

void class_code::print(base_obj *obj) {
    printf("<code object>");
}

struct_code::struct_code() {
    return;
}

void struct_code::print() {
    printf("<code object>");
}

void struct_code::set_code_obj(int argcount, int posonlyargcount, int kwonlyargcount, int nlocals, int stack_size, int flags,
                               int firstlineno, obj_str *bytecodes, inner_array<base_obj *> *consts,
                               inner_array<base_obj *> *names, inner_array<base_obj *> *var_names,
                               inner_array<base_obj *> *free_vars, inner_array<base_obj *> *cell_vars, obj_str *file_name,
                               obj_str *module_name, obj_str *lnotab, obj_str *file_path) {
    _argcount = argcount;
    _posonlyargcount = posonlyargcount;
    _kwonlyargcount = kwonlyargcount;
    _nlocals = nlocals;
    _stack_size = stack_size;
    _flags = flags;
    _firstlineno = firstlineno; 
    _bytecodes = bytecodes;
    _names = names;
    _consts = consts;
    _var_names = var_names;
    _free_vars = free_vars;
    _cell_vars = cell_vars;
    _module_name = module_name;
    _file_name = file_name;
    _lnotab = lnotab;
    _file_path = file_path;
    set_instance(class_code::return_instance());
}
