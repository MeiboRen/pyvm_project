#pragma once
#include "extern_objs/base_class.hpp"
#include "extern_objs/base_obj.hpp"

class class_closure : public base_class {
private:
    static class_closure *_instance;
public:
    class_closure();
    static class_closure *return_instance();
    virtual size_t size();
};

class struct_closure : public base_obj {
friend class class_closure;
private:
    int _index;
    struct_list *_list;
public:
    struct_closure(struct_list *list, int index);
    base_obj *value();
};
