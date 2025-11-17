#pragma once
#include "extern_objs/ctrl_structs/struct_func.hpp"
#include "extern_objs/base_obj.hpp"

class struct_dict;

class class_pkg : public base_class {
private:
    static class_pkg *_instance;
public:
    class_pkg();
    static class_pkg *return_instance();
    virtual size_t size();
    void initialize();
};

struct meth_info {
    const char *method_name, *method_doc;
    int method_info;
    struct_native_func method;
};

typedef meth_info* (*struct_func_init)();

class struct_pkg : public base_obj {
friend class class_pkg;
private:
    obj_str *_module_name;
public:
    struct_pkg(struct_dict *obj);
    base_obj *get(base_obj *obj);
    void put(base_obj *obj1, base_obj *obj2);
    void extend(struct_pkg *module_obj);
    static struct_pkg *import_module(obj_str *cursor_path, base_obj *obj);
    static struct_pkg *import_so(obj_str *module_name);
};
