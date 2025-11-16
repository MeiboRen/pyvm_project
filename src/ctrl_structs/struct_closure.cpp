#include "ctrl_structs/struct_closure.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_closure *class_closure::_instance = NULL;

class_closure::class_closure() {
    set_name(new obj_str("cell"));
    set_instance_dict(new struct_dict());
}

class_closure *class_closure::return_instance() {
    if (_instance == NULL) _instance = new class_closure();
    return _instance;
}

size_t class_closure::size() {
    return sizeof(class_closure);
}

struct_closure::struct_closure(struct_list *list, int index) {
    _list = list;
    _index = index;
    set_instance(class_closure::return_instance());
}

base_obj *struct_closure::value() {
    return _list->get(_index);
}
