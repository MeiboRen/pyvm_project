#pragma once
#include "builtin_objs/inner_array.hpp"
#include "extern_objs/base_obj.hpp"
#include "extern_objs/base_class.hpp"

class class_list : public base_class {
private:
    static class_list *instance;
public:
    class_list();
    static class_list *return_instance();
    virtual size_t size();
    void initialize();
    virtual void print(base_obj *obj);
    virtual base_obj *create_instance(base_obj* callable, inner_array<base_obj *> *args);
    virtual base_obj* greater(base_obj *obj1, base_obj *obj2);
    virtual base_obj *less(base_obj *obj1, base_obj *obj2);
    virtual base_obj *add(base_obj *obj1, base_obj *obj2);
    virtual base_obj *mul(base_obj *obj1, base_obj *obj2);
    virtual base_obj *len(base_obj *obj);
    virtual base_obj *contains(base_obj *obj1, base_obj *obj2);
    virtual base_obj *subscr(base_obj *obj1, base_obj *obj2);
    virtual void store_subscr(base_obj *obj1, base_obj *obj2, base_obj *obj3);
    virtual void delete_subscr(base_obj *obj1, base_obj *obj2);
    virtual base_obj *iter(base_obj *obj);
};

class struct_list : public base_obj {
friend class class_list;
private:
    inner_array<base_obj *> *_inner_list;
public:
    struct_list();
    struct_list(base_obj_array val_list);
    inner_array<base_obj*>* get_inner_list();
    int size();
    void append(base_obj* obj);
    void insert(int index, base_obj* obj);
    base_obj* top();
    base_obj* pop();
    base_obj* get(int index);
    void set(int index, base_obj* obj);
    int index(base_obj *obj);
    void delete_element(int index);
};

class class_list_iter : public base_class {
private:
    static class_list_iter *instance;
public:
    class_list_iter();
    static class_list_iter *return_instance();
};

class obj_list_iter : public base_obj {
private:
    struct_list* _container;
    int _iter_num;
public:
    obj_list_iter(struct_list *container);
    struct_list* get_container();
    int get_iter_num();
    void add_iter_num();
};

base_obj *list_append(base_obj_array args);
base_obj *list_insert(base_obj_array args);
base_obj *list_index(base_obj_array args);
base_obj *list_pop(base_obj_array args);
base_obj *list_pop_left(base_obj_array args);
base_obj *list_remove(base_obj_array args);
base_obj *list_reverse(base_obj_array args);
base_obj *list_sort(base_obj_array args);
base_obj *list_iter_next(base_obj_array args);
