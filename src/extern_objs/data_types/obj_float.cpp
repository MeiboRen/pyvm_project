#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "extern_objs/base_class.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_float.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_float *class_float::instance = NULL;

class_float::class_float() {
    return;
}

class_float *class_float::return_instance() {
    if (instance == NULL) instance = new class_float();
    return instance;
}

size_t class_float::size() {
    return sizeof(obj_float);
}

void class_float::print(base_obj *obj) {
    obj_float *val = (obj_float *)obj;
    assert(val && (val->get_instance()) == ((base_class *)this));
    printf("%g", val->get_value());
}

void class_float::initialize() {
    set_instance_dict(new struct_dict());
    set_name(new obj_str("float"));
    add_super_list(class_obj::return_instance());
    (new obj_type())->save_instance(this);
}

base_obj *class_float::create_instance(base_obj *callable, inner_array<base_obj *> *args) {
    if (!args || args->length() == 0) return new obj_float(0.0);
    if (args->length() == 1) {
        base_obj* arg = args->get(0);
        if (arg->get_instance() == class_float::return_instance()) return arg;
        if (arg->get_instance() == class_int::return_instance()) {
            obj_int* int_arg = (obj_int*)arg;
            return new obj_float((double)int_arg->get_value());
        }
        if (arg->get_instance() == class_str::return_instance()) {
            obj_str* str_arg = (obj_str*)arg;
            char* endptr;
            double value = strtod(str_arg->get_value(), &endptr);
            if (endptr != str_arg->get_value()) return new obj_float(value);
        }
        return new obj_float(0.0);
    } else return NULL;
}

base_obj *class_float::greater(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_false;
    if (v1 > v2) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_float::less(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_false;
    if (v1 < v2) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_float::equal(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) {
        if (obj2->get_instance() == class_int::return_instance()) {
            double v1 = ((obj_float *)obj1)->get_value();
            int v2 = ((obj_int *)obj2)->get_value();
            if (v1 == (double)v2) return parser::obj_true;
            else return parser::obj_false;
        }
        return parser::obj_false;
    }
    double v1 = ((obj_float *)obj1)->get_value();
    double v2 = ((obj_float *)obj2)->get_value();
    if (v1 == v2) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_float::not_equal(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) {
        if (obj2->get_instance() == class_int::return_instance()) {
            double v1 = ((obj_float *)obj1)->get_value();
            int v2 = ((obj_int *)obj2)->get_value();
            if (v1 != (double)v2) return parser::obj_true;
            else return parser::obj_false;
        }
        return parser::obj_true;
    }
    double v1 = ((obj_float *)obj1)->get_value();
    double v2 = ((obj_float *)obj2)->get_value();
    if (v1 != v2) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_float::ge(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_false;
    if (v1 >= v2) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_float::le(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_false;
    if (v1 <= v2) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *class_float::add(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_none;
    return new obj_float(v1 + v2);
}

base_obj *class_float::sub(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_none;
    return new obj_float(v1 - v2);
}

base_obj *class_float::mul(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_none;
    return new obj_float(v1 * v2);
}

base_obj *class_float::div(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_none;
    if (v2 == 0.0) {
        base_obj *zero_division_error = executor::return_instance()->get_builtins()->get(new obj_str("ZeroDivisionError"));
        base_obj *error_msg = new obj_str("division by zero");
        executor::return_instance()->do_raise(zero_division_error, error_msg, NULL);
        return NULL;
    }
    return new obj_float(v1 / v2);
}

base_obj *class_float::mod(base_obj *obj1, base_obj *obj2) {
    double v1 = ((obj_float *)obj1)->get_value();
    double v2;
    if (obj2->get_instance() == class_float::return_instance()) v2 = ((obj_float *)obj2)->get_value();
    else if (obj2->get_instance() == class_int::return_instance()) v2 = (double)((obj_int *)obj2)->get_value();
    else return parser::obj_none;
    if (v2 == 0.0) {
        base_obj *zero_division_error = executor::return_instance()->get_builtins()->get(new obj_str("ZeroDivisionError"));
        base_obj *error_msg = new obj_str("division by zero");
        executor::return_instance()->do_raise(zero_division_error, error_msg, NULL);
        return NULL;
    }
    return new obj_float(fmod(v1, v2));
}

base_obj *class_float::to_str(base_obj *obj) {
    obj_float *val = (obj_float *)obj;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%g", val->get_value());
    return new obj_str(buffer);
}

obj_float::obj_float(double val) {
    _value = val;
    set_instance(class_float::return_instance());
}

double obj_float::get_value() {
    return _value;
}