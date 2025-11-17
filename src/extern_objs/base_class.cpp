#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "extern_objs/ctrl_structs/struct_func.hpp"
#include "extern_objs/base_obj.hpp"
#include "extern_objs/base_class.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

base_class::base_class() {
    parser::array_class->add(this);
    _name = NULL;
    _super_list = NULL;
    _method_resolution_order = NULL;
    _instance_dict = NULL;
}

size_t base_class::size() {
    return sizeof(base_obj);
}

obj_str *base_class::get_name() {
    return _name;
}

void base_class::set_name(obj_str *obj) {
    _name = obj;
}

obj_type *base_class::get_super_list() {
    if (_super_list == NULL || _super_list->size() <= 0) return NULL;
    return (obj_type *)_super_list->get(0);
}

void base_class::set_super_list(struct_list *obj) {
    _super_list = obj;
}

void base_class::add_super_list(base_class *class_instance) {
    if (_super_list == NULL) _super_list = new struct_list();
    _super_list->append(class_instance->get_type_object());
}

void base_class::order_super_list() {
    if (_super_list == NULL) return;
    if (_method_resolution_order == NULL) _method_resolution_order = new struct_list();
    int cursor = -1;
    for (int i = 0; i < _super_list->size(); i++) {
        obj_type *type_obj = (obj_type *)(_super_list->get(i));
        base_class *instance_class = type_obj->get_own_instance();
        _method_resolution_order->append(type_obj);
        if (instance_class->mro() == NULL) continue;
        for (int j = 0; j < instance_class->mro()->size(); j++) {
            obj_type *type_obj = (obj_type *)(instance_class->mro()->get(j));
            int index = _method_resolution_order->index(type_obj);
            if (index < cursor) assert(false);
            cursor = index;
            if (index >= 0) _method_resolution_order->delete_element(index);
            _method_resolution_order->append(type_obj);
        }
    }
}

struct_list *base_class::mro() {
    return _method_resolution_order;
}

obj_type *base_class::get_type_object() {
    return _type_object;
}

void base_class::set_type_object(obj_type *obj) {
    _type_object = obj;
}

struct_dict *base_class::get_instance_dict() {
    return _instance_dict;
}

void base_class::set_instance_dict(struct_dict *dict) {
    _instance_dict = dict;
}

void base_class::print(base_obj *obj) {
    base_obj *methed_repr = get_instance_attr(obj, class_str_init::return_instance()->repr_str);
    if (methed_repr != parser::obj_none) {
        executor::return_instance()->call_virtual(methed_repr, NULL)->print();
        return;
    }
    printf("<object of ");
    obj->get_instance()->get_name()->print();
    printf(", at %p>", obj);
}

base_obj *base_class::build_instance(base_obj *obj, base_obj *supers, base_obj *name) {
    assert(obj->get_instance() == (base_class *) class_dict::return_instance());
    assert(supers->get_instance() == (base_class *) class_list::return_instance());
    assert(name->get_instance() == (base_class *) class_str::return_instance());

    base_class *new_instance = new base_class();
    struct_dict *class_instance_dict = (struct_dict *)obj;
    struct_list *supers_list = (struct_list *)supers;

    new_instance->set_name((obj_str *)name);
    new_instance->set_instance_dict(class_instance_dict);
    new_instance->set_super_list(supers_list);
    new_instance->order_super_list();

    obj_type *type_obj = new obj_type();
    type_obj->save_instance(new_instance);
    return type_obj;
}

int base_class::compare_instance(base_class* obj1, base_class* obj2) {
    if (obj1 == obj2) return 0;
    if (obj1 == class_int::return_instance()) return -1;
    else if (obj2 == class_int::return_instance()) return 1;
    if (obj1->get_name()->less(obj2->get_name()) == (base_obj *)parser::obj_true) return -1;
    else return 1;
}

base_obj *base_class::create_instance(base_obj *callable, inner_array<base_obj *> *args) {
    base_obj *instance = new base_obj();
    instance->set_instance(((obj_type *)callable)->get_own_instance());
    base_obj *constructor = instance->get_attr(class_str_init::return_instance()->init_str);
    if (constructor != parser::obj_none) executor::return_instance()->call_virtual(constructor, args);
    return instance;
}

base_obj *base_class::get_instance_attr(base_obj *obj1, base_obj *obj2) {
    base_obj *result = parser::obj_none;
    result = find_in_parents(obj1, obj2);
    if (struct_method::is_function(result)) result = new struct_method((struct_func *)result, obj1);
    return result;
}

base_obj *base_class::find_and_call(base_obj *obj, base_obj_array args, base_obj *func_name) {
    base_obj *func = obj->get_attr(func_name);
    if (func != parser::obj_none) return executor::return_instance()->call_virtual(func, args);
    assert(false);
    return parser::obj_none;
}

base_obj *base_class::find_in_parents(base_obj *obj1, base_obj *obj2) {
    base_obj *result = parser::obj_none;
    result = obj1->get_instance()->get_instance_dict()->get(obj2);
    if (result != parser::obj_none || obj1->get_instance()->mro() == NULL) return result;
    for (int i = 0; i < obj1->get_instance()->mro()->size(); i++) {
        obj_type *type_obj = (obj_type *)(obj1->get_instance()->mro()->get(i));
        result = type_obj->get_own_instance()->get_instance_dict()->get(obj2);
        if (result != parser::obj_none) return result;
    }
    return result;
}

base_obj* base_class::greater(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj* base_class::less(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj* base_class::equal(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj* base_class::not_equal(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj* base_class::ge(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj* base_class::le(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj *base_class::add(base_obj *obj1, base_obj *obj2) {
    base_obj_array args = new inner_array<base_obj *>();
    args->add(obj2);
    return find_and_call(obj1, args, class_str_init::return_instance()->add_str);
}

base_obj *base_class::sub(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj *base_class::mul(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj *base_class::div(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj *base_class::mod(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj *base_class::len(base_obj *obj) {
    return find_and_call(obj, NULL, class_str_init::return_instance()->len_str);
}

base_obj *base_class::contains(base_obj *obj1, base_obj *obj2) {
    return 0;
}

base_obj *base_class::subscr(base_obj *obj1, base_obj *obj2) {
    base_obj_array args = new inner_array<base_obj *>();
    args->add(obj2);
    return find_and_call(obj1, args, class_str_init::return_instance()->getitem_str);
}

void base_class::store_subscr(base_obj *obj1, base_obj *obj2, base_obj *obj3) {
    base_obj_array args = new inner_array<base_obj *>();
    args->add(obj2);
    args->add(obj3);
    find_and_call(obj1, args, class_str_init::return_instance()->setitem_str);
}

void base_class::delete_subscr(base_obj *obj1, base_obj *obj2) {
    return;
}

base_obj *base_class::get_attr(base_obj *obj1, base_obj *obj2) {
    base_obj *func = find_in_parents(obj1, class_str_init::return_instance()->getattr_str);
    if (func->get_instance() == class_func::return_instance()) {
        func = new struct_method((struct_func *)func, obj1);
        base_obj_array args = new inner_array<base_obj *>();
        args->add(obj2);
        return executor::return_instance()->call_virtual(func, args);
    }
    base_obj *result = parser::obj_none;
    if (obj1->get_obj_dict() != NULL) {
        result = obj1->get_obj_dict()->get(obj2);
        if (result != parser::obj_none) return result;
    }
    return get_instance_attr(obj1, obj2);
}

base_obj *base_class::set_attr(base_obj *obj1, base_obj *obj2, base_obj *obj3) {
    base_obj *func = obj1->get_instance()->get_instance_dict()->get(class_str_init::return_instance()->setattr_str);
    if (func->get_instance() == class_func::return_instance()) {
        func = new struct_method((struct_func *)func, obj1);
        base_obj_array args = new inner_array<base_obj *>();
        args->add(obj2);
        args->add(obj3);
        return executor::return_instance()->call_virtual(func, args);
    }
    if (obj1->get_obj_dict() == NULL) obj1->init_obj_dict();
    obj1->get_obj_dict()->put(obj2, obj3);
    return parser::obj_none;
}

base_obj *base_class::next(base_obj *obj) {
    return find_and_call(obj, NULL, class_str_init::return_instance()->next_str);
}

base_obj *base_class::iter(base_obj *obj) {
    return find_and_call(obj, NULL, class_str_init::return_instance()->iter_str);
}

base_obj *base_class::to_str(base_obj *obj) {
    return obj;
}

void *base_class::operator new(size_t size) {
    return ::operator new(size);
}
