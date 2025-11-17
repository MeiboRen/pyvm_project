#pragma once
#include "extern_objs/base_obj.hpp"

class class_set : public base_class {
private:
    static class_set *instance;
public:
    class_set();
    static class_set *return_instance();
    virtual size_t size();
    void initialize();
    virtual void print(base_obj* obj);
    virtual base_obj *create_instance(base_obj* callable, inner_array<base_obj *> *args);

    virtual base_obj* equal(base_obj* obj1, base_obj* obj2);
    virtual base_obj* not_equal(base_obj* obj1, base_obj* obj2);
    virtual base_obj* len(base_obj* obj);
    virtual base_obj* contains(base_obj* obj1, base_obj* obj2);
    virtual base_obj* iter(base_obj* obj);
    virtual base_obj *to_str(base_obj *obj);
};

class struct_set : public base_obj {
private:
    inner_array<base_obj *> *_inner_set;
public:
    struct_set();
    struct_set(inner_array<base_obj *> *val_set);
    inner_array<base_obj*>* get_inner_set();
    int size();
    void add(base_obj* obj);
    void remove(base_obj* obj);
    bool contains(base_obj* obj);
    base_obj* pop();
    void update(base_obj* iterable);
    void clear();
    struct_set* union_with(struct_set* other);
    struct_set* intersection(struct_set* other);
    struct_set* difference(struct_set* other);
};

base_obj *set_add(base_obj_array args);
base_obj *set_remove(base_obj_array args);
base_obj *set_pop(base_obj_array args);
base_obj *set_clear(base_obj_array args);
base_obj *set_update(base_obj_array args);
base_obj *set_contains(base_obj_array args);
base_obj *set_union(base_obj_array args);
base_obj *set_intersection(base_obj_array args);
base_obj *set_difference(base_obj_array args);
