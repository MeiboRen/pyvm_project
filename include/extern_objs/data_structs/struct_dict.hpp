#pragma once
#include "builtin_objs/inner_dict.hpp"
#include "extern_objs/base_obj.hpp"

enum iter_type {
    ITER_KEY = 0,
    ITER_VALUE = 1,
    ITER_ITEM = 2
};

class class_dict : public base_class {
private:
    static class_dict* instance;
public:
    class_dict();
    static class_dict* return_instance();
    virtual size_t size();
    void initialize();
    virtual void print(base_obj* obj);
    virtual base_obj *create_instance(base_obj* callable, inner_array<base_obj *> *args);
    virtual base_obj* subscr(base_obj* obj1, base_obj* obj2);
    virtual void store_subscr(base_obj* obj1, base_obj* obj2, base_obj* obj3);
    virtual void delete_subscr(base_obj* obj1, base_obj* obj2);
    virtual base_obj* iter(base_obj* obj);
};

class struct_dict : public base_obj {
friend class class_dict;
private:
    inner_dict<base_obj*, base_obj*>* _map;
public:
    struct_dict();
    struct_dict(inner_dict<base_obj*, base_obj*>* map);
    inner_dict<base_obj*, base_obj*>* get_map();
    int size();
    bool has_key(base_obj* key);
    base_obj* get(base_obj* key);
    void put(base_obj* key, base_obj* val);
    base_obj* remove(base_obj* key);
    void update(struct_dict *dict);
};

template<iter_type T>
class class_dict_iter : public base_class {
private:
    static class_dict_iter *instance;
public:
    class_dict_iter();
    static class_dict_iter *return_instance();
    virtual base_obj *iter(base_obj *obj);
    virtual base_obj *next(base_obj *obj);
};

class obj_dict_iter : public base_obj {
private:
    struct_dict *_container;
    int _iter_num;
public:
    obj_dict_iter(struct_dict *container);
    struct_dict *get_container();
    int get_iter_num();
    void add_iter_num();
};

base_obj* dict_set_default(base_obj_array args);
base_obj* dict_remove(base_obj_array args);
base_obj* dict_pop(base_obj_array args);
base_obj* dict_keys(base_obj_array args);
base_obj* dict_values(base_obj_array args);
base_obj* dict_items(base_obj_array args);
base_obj* dict_iter_next(base_obj_array args);
