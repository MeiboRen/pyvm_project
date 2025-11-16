#pragma once
#include "builtin_objs/inner_dict.hpp"
#include "ctrl_structs/struct_code.hpp"
#include "extern_objs/base_class.hpp"

class class_func : public base_class {
private:
    static class_func *instance;
public:
    class_func();
    static class_func *return_instance();
    virtual size_t size();
    virtual void print(base_obj *obj);
};

typedef base_obj *(*struct_native_func)(base_obj_array args);

class struct_func : public base_obj {
friend class class_func;
friend class class_native_func;
friend class struct_call;
private:
    obj_str *_func_name, *_real_name;
    unsigned int _flags;
    struct_code *_func_code;
    struct_dict *_globals;
    base_obj_array _defaults;
    struct_list* _closure;
    struct_native_func _native_func;
public:
    struct_func(base_class *class_instance);
    struct_func(struct_native_func native_func_ptr);
    struct_func(base_obj *code_object, obj_str *qual_name);
    void print();
    obj_str *get_func_name();
    int get_flags();
    struct_dict *get_globals();
    void set_globals(struct_dict *obj);
    base_obj_array get_defaults();
    void set_defaults(base_obj_array defaults);
    struct_list *get_closure();
    void set_closure(struct_list *obj);
    base_obj *call(base_obj_array args);
};

class class_native_func : public base_class {
private:
    static class_native_func *instance;
public:
    class_native_func();
    static class_native_func *return_instance();
    virtual size_t size();
};
    
class class_method : public base_class {
private:
    static class_method* instance;
public:
    class_method();
    static class_method* return_instance();
    virtual size_t size();
};

class struct_method : public base_obj {
friend class class_method;
private:
    struct_func *_func;
    base_obj* _container;
public:
    struct_method(struct_func *func);
    struct_method(struct_func *func, base_obj *container);
    struct_func *get_func();
    base_obj *get_container();
    void set_container(base_obj *obj);
    static bool is_function(base_obj *obj);
};

base_obj* len(base_obj_array args);
base_obj* type(base_obj_array args);
base_obj* isinstance(base_obj_array args);
base_obj* iter(base_obj_array args);
base_obj* next(base_obj_array args);
base_obj* print(base_obj_array args);
base_obj *_build_class(base_obj_array args);
