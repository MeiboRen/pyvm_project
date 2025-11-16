#pragma once
#include "extern_objs/base_obj.hpp"

class obj_str;

class class_code : public base_class {
private:
    static class_code *instance;
public:
    class_code();
    static class_code *return_instance();
    virtual size_t size();
    virtual void print(base_obj* obj);
};

class struct_code : public base_obj {
public:
    int _argcount, _posonlyargcount, _kwonlyargcount, _nlocals, _stack_size, _flags, _firstlineno;
    inner_array<base_obj *> *_consts, *_names, *_var_names, *_free_vars, *_cell_vars;
    obj_str *_bytecodes, *_file_name, *_module_name, *_lnotab, *_file_path;
    struct_code();
    void print();
    void set_code_obj(int argcount, int posonlyargcount, int kwonlyargcount, int nlocals, int stack_size, int flags,
                     int firstlineno, obj_str *bytecodes, inner_array<base_obj *> *consts, inner_array<base_obj *> *names,
                     inner_array<base_obj *> *var_names, inner_array<base_obj *> *free_vars, inner_array<base_obj *> *cell_vars,
                     obj_str *file_name, obj_str *module_name, obj_str *lnotab, obj_str *file_path);
};

template<typename T> class inner_array;
