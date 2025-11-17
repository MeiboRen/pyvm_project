#pragma once
#include "extern_objs/base_obj.hpp"

class obj_str;
class struct_list;
class struct_call;

class class_tb_info : public base_class {
private:
    static class_tb_info *_instance;
public:
    class_tb_info();
    static class_tb_info *return_instance();
    virtual size_t size();
    virtual void print(base_obj *obj);
};

class tb_info : public base_obj {
friend class_tb_info;
private:
    obj_str *_file_name, *_func_name;
    int _line_no;
public:
    tb_info(obj_str *file_name, obj_str *func_name, int line_no);
};

class struct_tb_class : public base_class {
private:
    static struct_tb_class *_instance;
public:
    struct_tb_class();
    static struct_tb_class *return_instance();
    virtual size_t size();
    virtual void print(base_obj *obj);
};

class struct_tb : public base_obj {
friend class struct_tb_class;
private:
    struct_list *_tb_stack;
public:
    struct_tb();
    void record_frame(struct_call *frame);
};
