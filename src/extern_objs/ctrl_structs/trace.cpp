#include "extern_objs/ctrl_structs/struct_call.hpp"
#include "extern_objs/ctrl_structs/trace.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"

class_tb_info *class_tb_info::_instance = NULL;
struct_tb_class *struct_tb_class::_instance = NULL;

class_tb_info::class_tb_info() {
    return;
}

class_tb_info *class_tb_info::return_instance() {
    if (_instance == NULL) _instance = new class_tb_info();
    return _instance;
}

size_t class_tb_info::size() {
    return sizeof(tb_info);
}

void class_tb_info::print(base_obj *obj) {
    tb_info *tb_obj = (tb_info *)obj;
    printf("  File \"");
    tb_obj->_file_name->print();
    printf("\", line %d, in ", tb_obj->_line_no);
    tb_obj->_func_name->print();
    printf("\n");
}

tb_info::tb_info(obj_str *file_name, obj_str *func_name, int line_no) {
    _file_name = file_name;
    _func_name = func_name;
    _line_no = line_no;
    set_instance(class_tb_info::return_instance());
}

struct_tb_class::struct_tb_class() {
    return;
}

struct_tb_class *struct_tb_class::return_instance() {
    if (_instance == NULL) _instance = new struct_tb_class();
    return _instance;
}

size_t struct_tb_class::size() {
    return sizeof(struct_tb);
}

void struct_tb_class::print(base_obj *obj) {
    struct_tb *tb_obj = (struct_tb *) obj;
    printf("Traceback (most recent call last):\n");
    for (int i = tb_obj->_tb_stack->size() - 1; i >= 0; i--) tb_obj->_tb_stack->get(i)->print();
}

struct_tb::struct_tb() {
    _tb_stack = new struct_list();
    set_instance(struct_tb_class::return_instance());
}

void struct_tb::record_frame(struct_call *frame) {
    _tb_stack->append(new tb_info(frame->file_name(), frame->func_name(), frame->current_lineno()));
}
