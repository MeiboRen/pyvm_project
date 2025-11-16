#include <stdio.h>
#include <stdlib.h>
#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "extern_objs/base_class.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_int *class_int::instance = NULL;

class_int::class_int() {
    return;
}

class_int *class_int::return_instance() {
    if (instance == NULL) instance = new class_int();
    return instance;
}

size_t class_int::size() {
    return sizeof(obj_int);
}

void class_int::print(base_obj *obj) {
    obj_int *val = (obj_int *)obj;
    assert(val && (val->get_instance()) == ((base_class *)this));
    printf("%d", val->get_value());
}

void class_int::initialize() {
    set_instance_dict(new struct_dict());
    set_name(new obj_str("int"));
    add_super_list(class_obj::return_instance());
    (new obj_type())->save_instance(this);
}

base_obj *class_int::create_instance(base_obj *callable, inner_array<base_obj *> *args) {
    if (!args || args->length() == 0) return new obj_int(0);
    if (args->length() == 1) {
        base_obj* arg = args->get(0);
        if (arg->get_instance() == class_int::return_instance()) return arg;
        if (arg->get_instance() == class_str::return_instance()) {
            obj_str* str_arg = (obj_str*)arg;
            char* endptr;
            long value = strtol(str_arg->get_value(), &endptr, 10);
            if (endptr != str_arg->get_value()) return new obj_int((int)value);
        }
        return new obj_int(0);
    } else return NULL;
}

base_obj *class_int::greater(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (val1->get_value() > val2->get_value()) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_int::less(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (obj1->get_instance() != obj2->get_instance()) {
        if (base_class::compare_instance(obj1->get_instance(), obj2->get_instance()) < 0) return parser::obj_true;
        else return parser::obj_false;
    }
    if (val1->get_value() < val2->get_value()) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_int::equal(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) return parser::obj_false;
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (val1->get_value() == val2->get_value()) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_int::not_equal(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (val1->get_value() != val2->get_value()) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_int::ge(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (val1->get_value() >= val2->get_value()) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_int::le(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (val1->get_value() <= val2->get_value()) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_int::add(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    return new obj_int(val1->get_value() + val2->get_value());
}

base_obj *class_int::sub(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    return new obj_int(val1->get_value() - val2->get_value());
}

base_obj *class_int::mul(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    return new obj_int(val1->get_value() * val2->get_value());
}

base_obj *class_int::div(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (val2->get_value() == 0) {
        base_obj *zero_division_error = executor::return_instance()->get_builtins()->get(new obj_str("ZeroDivisionError"));
        base_obj *error_msg = new obj_str("division by zero");
        executor::return_instance()->do_raise(zero_division_error, error_msg, NULL);
        return NULL;
    }
    return new obj_int(val1->get_value() / val2->get_value());
}

base_obj *class_int::mod(base_obj *obj1, base_obj *obj2) {
    obj_int *val1 = (obj_int *)obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));
    if (val2->get_value() == 0) {
        base_obj *zero_division_error = executor::return_instance()->get_builtins()->get(new obj_str("ZeroDivisionError"));
        base_obj *error_msg = new obj_str("division by zero");
        executor::return_instance()->do_raise(zero_division_error, error_msg, NULL);
        return NULL;
    }
    return new obj_int(val1->get_value() % val2->get_value());
}

base_obj *class_int::to_str(base_obj *obj) {
    obj_int *val = (obj_int *)obj;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", val->get_value());
    return new obj_str(buffer);
}

obj_int::obj_int(int val) {
    _value = val;
    set_instance(class_int::return_instance());
}

int obj_int::get_value() {
    return _value;
};
