#pragma once
#include "extern_objs/base_obj.hpp"

class class_str : public base_class {
private:
    static class_str *instance;
public:
    class_str();
    static class_str *return_instance();
    virtual size_t size();
    void initialize();
    virtual void print(base_obj *obj);
    virtual base_obj *create_instance(base_obj *callable, inner_array<base_obj *> *args);

    virtual base_obj* greater(base_obj *obj1, base_obj *obj2);
    virtual base_obj *less(base_obj *obj1, base_obj *obj2);
    virtual base_obj *equal(base_obj *obj1, base_obj *obj2);
    virtual base_obj* not_equal(base_obj *obj1, base_obj *obj2);
    virtual base_obj *add(base_obj *obj1, base_obj *obj2);
    virtual base_obj *len(base_obj *obj);
    virtual base_obj *contains(base_obj *obj1, base_obj *obj2);
    virtual base_obj *subscr(base_obj *obj1, base_obj *obj2);
    virtual base_obj *iter(base_obj *obj);
};

class obj_str : public base_obj {
friend class class_str;
private:
    char *_value;
    int _length;
public:
    obj_str(const char *obj);
    obj_str(const int len);
    obj_str(const char *obj, const int length);
    char *get_value();
    char **get_value_address();
    int get_length();
    void set_pos(int pos, char obj);
    obj_str *join(base_obj *iterable);
};

class class_str_iter : public base_class {
private:
    static class_str_iter *instance;
public:
    class_str_iter();
    static class_str_iter *return_instance();
};

class obj_str_iter : public base_obj {
private:
    obj_str* _container;
    int _iter_num;
public:
    obj_str_iter(obj_str *container);
    obj_str* get_container();
    int iter_num();
    void add_iter_num();
};

class class_str_init {
private:
    static class_str_init *instance;
public:
    obj_str *empty_str;
    obj_str *next_str;
    obj_str *mod_str;
    obj_str *init_str;
    obj_str *add_str;
    obj_str *len_str;
    obj_str *call_str;
    obj_str *name_str;
    obj_str *iter_str;
    obj_str *repr_str;
    obj_str *getitem_str;
    obj_str *setitem_str;
    obj_str *getattr_str;
    obj_str *setattr_str;
    obj_str *so_pre_str;
    obj_str *libdir_pre_str;
    obj_str *so_suf_str;
    obj_str *pyc_suf_str;
    class_str_init();
    static class_str_init *return_instance();
};

base_obj *str_upper(base_obj_array args);
base_obj *str_join(base_obj_array args);
base_obj *str_iter_next(base_obj_array args);
