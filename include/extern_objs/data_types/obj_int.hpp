#pragma once
#include "extern_objs/base_obj.hpp"
#include "extern_objs/data_types/obj_str.hpp"

class class_int : public base_class {
private:
    static class_int *instance;
public:
    class_int();
    static class_int *return_instance();
    virtual size_t size();
    virtual void print(base_obj* obj);
    void initialize();
    virtual base_obj *create_instance(base_obj* callable, inner_array<base_obj *> *args);

    virtual base_obj* greater(base_obj* obj1, base_obj* obj2);
    virtual base_obj* less(base_obj* obj1, base_obj* obj2);
    virtual base_obj* equal(base_obj* obj1, base_obj* obj2);
    virtual base_obj* not_equal(base_obj* obj1, base_obj* obj2);
    virtual base_obj* ge(base_obj* obj1, base_obj* obj2);
    virtual base_obj* le(base_obj* obj1, base_obj* obj2);
    virtual base_obj* add(base_obj* obj1, base_obj* obj2);
    virtual base_obj* sub(base_obj* obj1, base_obj* obj2);
    virtual base_obj* mul(base_obj* obj1, base_obj* obj2);
    virtual base_obj* div(base_obj* obj1, base_obj* obj2);
    virtual base_obj* mod(base_obj* obj1, base_obj* obj2);
    virtual base_obj *to_str(base_obj *obj);
};

class obj_int : public base_obj {
private:
    int _value;
public:
    obj_int(int val);
    int get_value();
};
