#pragma once
#include "extern_objs/base_class.hpp"

class obj_str;

class class_obj : public base_class {
private:
    static class_obj *instance;
public:
    class_obj();
    static class_obj *return_instance();
};

class base_obj {
private:
    base_class *_instance;
    struct_dict *_obj_dict;
public:
    base_class *get_instance();
    void set_instance(base_class *obj);
    size_t size();
    void print();
    struct_dict *get_obj_dict();
    void set_obj_dict(struct_dict *obj);
    void init_obj_dict();

    base_obj *greater(base_obj *obj);
    base_obj *less(base_obj *obj);
    base_obj *equal(base_obj *obj);
    base_obj *not_equal(base_obj *obj);
    base_obj *ge(base_obj *obj);
    base_obj *le(base_obj *obj);
    base_obj *add(base_obj *obj);
    base_obj *sub(base_obj *obj);
    base_obj *mul(base_obj *obj);
    base_obj *div(base_obj *obj);
    base_obj *mod(base_obj *obj);
    base_obj *len();
    base_obj *contains(base_obj *obj);
    base_obj *subscr(base_obj *obj);
    void store_subscr(base_obj *obj1, base_obj *obj2);
    void delete_subscr(base_obj *obj);
    base_obj *get_attr(base_obj *obj);
    base_obj *set_attr(base_obj *obj1, base_obj *obj2);
    base_obj *next();
    base_obj *iter();
    void *operator new(size_t size);
};

class class_type : public base_class {
private:
    static class_type *instance;
public:
    class_type();
    static class_type *return_instance();
    virtual size_t size();
    virtual void print(base_obj *obj);
    virtual base_obj *set_attr(base_obj *obj1, base_obj *obj2, base_obj *obj3);
};

class obj_type : public base_obj {
private:
    base_class *_own_instance;
public:
    obj_type();
    base_class *get_own_instance();
    void save_instance(base_class *val_class);
};
