#pragma once
#include <stdio.h>
#include <assert.h>
#include "builtin_objs/inner_array.hpp"

class base_obj;
class obj_type;
class obj_str;
class struct_list;
class struct_dict;

class base_class {
private:
    obj_str * _name;
    struct_list *_super_list, *_method_resolution_order;
    obj_type *_type_object;
    struct_dict* _instance_dict;
public:
    base_class();
    virtual size_t size();
    obj_str *get_name();
    void set_name(obj_str *obj);
    obj_type *get_super_list();
    void set_super_list(struct_list *obj);
    void add_super_list(base_class *obj);
    void order_super_list();
    struct_list *mro();
    obj_type *get_type_object();
    void set_type_object(obj_type *obj);
    struct_dict *get_instance_dict();
    void set_instance_dict(struct_dict *dict);
    virtual void print(base_obj *obj);
    static base_obj *build_instance(base_obj *obj, base_obj *supers, base_obj *name);
    static int compare_instance(base_class* obj1, base_class* obj2);
    virtual base_obj *create_instance(base_obj* callable, inner_array<base_obj *> *args);
    virtual base_obj *get_instance_attr(base_obj *obj1, base_obj *obj2);
    base_obj *find_and_call(base_obj *obj, base_obj_array args, base_obj *func_name);
    base_obj *find_in_parents(base_obj *obj1, base_obj *obj2);

    virtual base_obj* greater(base_obj *obj1, base_obj *obj2);
    virtual base_obj* less(base_obj *obj1, base_obj *obj2);
    virtual base_obj* equal(base_obj *obj1, base_obj *obj2);
    virtual base_obj* not_equal(base_obj *obj1, base_obj *obj2);
    virtual base_obj* ge(base_obj *obj1, base_obj *obj2);
    virtual base_obj* le(base_obj *obj1, base_obj *obj2);
    virtual base_obj* add(base_obj *obj1, base_obj *obj2);
    virtual base_obj* sub(base_obj *obj1, base_obj *obj2);
    virtual base_obj* mul(base_obj *obj1, base_obj *obj2);
    virtual base_obj* div(base_obj *obj1, base_obj *obj2);
    virtual base_obj* mod(base_obj *obj1, base_obj *obj2);
    virtual base_obj *len(base_obj *obj);
    virtual base_obj *contains(base_obj *obj1, base_obj *obj2);
    virtual base_obj *subscr(base_obj *obj1, base_obj *obj2);
    virtual void store_subscr(base_obj *obj1, base_obj *obj2, base_obj *obj3);
    virtual void delete_subscr(base_obj *obj1, base_obj *obj2);
    virtual base_obj *get_attr(base_obj *obj1, base_obj *obj2);
    virtual base_obj *set_attr(base_obj *obj1, base_obj *obj2, base_obj *obj3);
    virtual base_obj *next(base_obj* obj);
    virtual base_obj *iter(base_obj* obj);
    virtual base_obj *to_str(base_obj *obj);
    void* operator new(size_t size);
};
