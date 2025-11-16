#include <stdio.h>
#include <string.h>
#include "vm/parser.hpp"
#include "ctrl_structs/struct_func.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_str *class_str::instance = NULL;
class_str_iter *class_str_iter::instance = NULL;
class_str_init *class_str_init::instance = NULL;

class_str::class_str() {
    return;
}

class_str *class_str::return_instance() {
    if (instance == NULL) instance = new class_str();
    return instance;
}

size_t class_str::size() {
    return sizeof(obj_str);
}

void class_str::initialize() {
    //(new obj_type())->save_instance(this);
    struct_dict *class_instance_dict = new struct_dict();
    class_instance_dict->put(new obj_str("upper"), new struct_func(str_upper));
    class_instance_dict->put(new obj_str("join"), new struct_func(str_join));
    set_instance_dict(class_instance_dict);
    class_str_iter::return_instance();
    set_name(new obj_str("str"));
    add_super_list(class_obj::return_instance());
    (new obj_type())->save_instance(this);
}

void class_str::print(base_obj *obj) {
    obj_str *str_obj = (obj_str *)obj;
    assert(str_obj && str_obj->get_instance() == (base_class *)this);
    for (int i = 0; i < str_obj->get_length(); i++) printf("%c", str_obj->get_value()[i]);
}

base_obj *class_str::create_instance(base_obj *callable, inner_array<base_obj *> *args) {
    if (!args || args->length() == 0) return new obj_str("");
    if (args->length() == 1) {
        base_obj* arg = args->get(0);
        if (arg->get_instance() == class_str::return_instance()) return arg;
        if (arg->get_instance() == class_int::return_instance()) {
            obj_int* int_arg = (obj_int*)arg;
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", int_arg->get_value());
            return new obj_str(buffer);
        }
        else return new obj_str("<object>");
    } else return NULL;
}

base_obj *class_str::greater(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) {
        if (base_class::compare_instance(obj1->get_instance(), obj2->get_instance()) < 0) return parser::obj_false;
        else return parser::obj_true;
    }
    obj_str *val1 = (obj_str *) obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_str *val2 = (obj_str *) obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));

    int len = val1->get_length() < val2->get_length() ? val1->get_length() : val2->get_length();
    for (int i = 0; i < len; i++) {
        if (val1->get_value()[i] < val2->get_value()[i]) return parser::obj_false;
        else if (val1->get_value()[i] > val2->get_value()[i]) return parser::obj_true;
    }
    if (val1->get_length() < val2->get_length()) return parser::obj_false;
    return parser::obj_true;
}

base_obj *class_str::less(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) {
        if (base_class::compare_instance(obj1->get_instance(), obj2->get_instance()) < 0) return parser::obj_true;
        else return parser::obj_false;
    }
    obj_str *val1 = (obj_str *) obj1;
    assert(val1 && (val1->get_instance() == (base_class *)this));
    obj_str *val2 = (obj_str *) obj2;
    assert(val2 && (val2->get_instance() == (base_class *)this));

    int len = val1->get_length() < val2->get_length() ? val1->get_length() : val2->get_length();
    for (int i = 0; i < len; i++) {
        if (val1->get_value()[i] < val2->get_value()[i]) return parser::obj_true;
        else if (val1->get_value()[i] > val2->get_value()[i]) return parser::obj_false;
    }
    if (val1->get_length() < val2->get_length()) return parser::obj_true;
    return parser::obj_false;
}

base_obj *class_str::equal(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) return parser::obj_false;
    obj_str *val1 = (obj_str *) obj1;
    assert(val1 && val1->get_instance() == (base_class *) this);
    obj_str *val2 = (obj_str *) obj2;
    assert(val2 && val2->get_instance() == (base_class *) this);

    if (val1->get_length() != val2->get_length()) return parser::obj_false;
    for (int i = 0; i < val1->get_length(); i++)
        if (val1->get_value()[i] != val2->get_value()[i])
            return parser::obj_false;
    return parser::obj_true;
}

base_obj *class_str::not_equal(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) return parser::obj_true;
    obj_str *val1 = (obj_str *) obj1;
    assert(val1 && val1->get_instance() == (base_class *) this);
    obj_str *val2 = (obj_str *) obj2;
    assert(val2 && val2->get_instance() == (base_class *) this);

    if (val1->get_length() != val2->get_length()) return parser::obj_true;
    for (int i = 0; i < val1->get_length(); i++)
        if (val1->get_value()[i] != val2->get_value()[i])
            return parser::obj_true;
    return parser::obj_false;
}

base_obj *class_str::add(base_obj *obj1, base_obj *obj2) {
    assert(obj1 && obj1->get_instance() == this);
    assert(obj2 && obj2->get_instance() == this);
    obj_str *val1 = (obj_str *) obj1;
    obj_str *val2 = (obj_str *) obj2;
    obj_str *val3 = new obj_str(val1->get_length() + val2->get_length());
    memcpy(val3->_value, val1->_value, val1->get_length());
    memcpy(val3->_value + val1->get_length(), val2->_value, val2->get_length());
    val3->set_pos(val1->get_length() + val2->get_length(), '\0');
    return val3;
}

base_obj *class_str::len(base_obj *obj) {
    return new obj_int(((obj_str *)obj)->get_length());
}

base_obj *class_str::contains(base_obj *obj1, base_obj *obj2) {
    obj_str *val1 = (obj_str *)obj1;
    assert(val1 && val1->get_instance() == (base_class *)this);
    obj_str *val2 = (obj_str *)obj2;
    assert(val2 && val2->get_instance() == (base_class *)this);
    for (int i = 0; i < val1->get_length(); i++) {
        if (val1->get_length() - i < val2->get_length()) return parser::obj_false;
        for (int j = 0; j < val2->get_length(); j++) {
            if (val1->get_value()[i + j] != val2->get_value()[j]) break;
            if (j == val2->get_length() - 1) return parser::obj_true;
        }
    }
    return parser::obj_false;
}

base_obj *class_str::subscr(base_obj *obj1, base_obj *obj2) {
    assert(obj1 && obj1->get_instance() == (base_class *)this);
    assert(obj2 && obj2->get_instance() == (base_class *)class_int::return_instance());
    obj_str *val1 = (obj_str *) obj1;
    obj_int *val2 = (obj_int *) obj2;
    return new obj_str(&(val1->get_value()[val2->get_value()]), 1);
}

base_obj *class_str::iter(base_obj *obj) {
    assert(obj && obj->get_instance() == (base_class *)this);
    return new obj_str_iter((obj_str *)obj);
}

obj_str::obj_str(const char *obj) {
    _length = strlen(obj);
    _value = new char[_length];
    strcpy(_value, obj);
    set_instance(class_str::return_instance());
}

obj_str::obj_str(const int length) {
    _length = length;
    _value = new char[_length + 1];
    set_instance(class_str::return_instance());
}

obj_str::obj_str(const char *obj, const int length) {
    _length = length;
    _value = new char[_length];
    for (int i = 0; i < length; i++) _value[i] = obj[i];
    set_instance(class_str::return_instance());
}

char *obj_str::get_value() {
    return _value;
}

char **obj_str::get_value_address() {
    return &_value;
}

int obj_str::get_length() {
    return _length;
}

void obj_str::set_pos(int pos, char obj) {
    _value[pos] = obj;
}

obj_str *obj_str::join(base_obj *iterable) {
    int total = 0;
    base_obj *iter = iterable->iter();
    base_obj *str = iter->next();
    if (str == NULL) return new obj_str("");
    total += ((obj_str *)str)->get_length();
    while ((str = iter->next()) != NULL) total = total + _length + ((obj_str *)str)->get_length();

    total = 0;
    iter = iterable->iter();
    str = iter->next();
    obj_str *val3 = new obj_str(total);
    obj_str *str_obj = (obj_str *)str;
    memcpy(val3->_value, str_obj->_value, str_obj->get_length());
    total += ((obj_str *)str)->get_length();
    while ((str = iter->next()) != NULL) {
        obj_str *str_obj = (obj_str *)str;
        memcpy(val3->_value + total, _value, _length);
        total += _length;
        memcpy(val3->_value + total, str_obj->_value, str_obj->get_length());
        total += str_obj->_length;
    }
    return val3;
}

class_str_iter::class_str_iter() {
    set_name(new obj_str("str_iterator"));
    struct_dict *class_instance_dict = new struct_dict();
    class_instance_dict->put(new obj_str("__next__"), new struct_func(str_iter_next));
    set_instance_dict(class_instance_dict);
}

class_str_iter *class_str_iter::return_instance() {
    if (instance == NULL) instance = new class_str_iter();
    return instance;
}

obj_str_iter::obj_str_iter(obj_str *str) {
    _container = str;
    _iter_num = 0;
    set_instance(class_str_iter::return_instance());
}

obj_str* obj_str_iter::get_container() {
    return _container;
}

int obj_str_iter::iter_num() {
    return _iter_num;
}

void obj_str_iter::add_iter_num() {
    _iter_num++;
}

class_str_init::class_str_init() {
    empty_str = new obj_str("");
    next_str = new obj_str("__next__");
    mod_str = new obj_str("__module__");
    init_str = new obj_str("__init__");
    add_str = new obj_str("__add__");
    len_str = new obj_str("__len__");
    call_str = new obj_str("__call__");
    name_str = new obj_str("__name__");
    iter_str = new obj_str("__iter__");
    repr_str = new obj_str("__repr__");
    getitem_str = new obj_str("__getitem__");
    setitem_str = new obj_str("__setitem__");
    getattr_str = new obj_str("__getattr__");
    setattr_str = new obj_str("__setattr__");
    so_pre_str = new obj_str("lib");
    libdir_pre_str = new obj_str("./lib/");
    so_suf_str = new obj_str(".so");
    pyc_suf_str = new obj_str(".pyc");
}

class_str_init *class_str_init::return_instance() {
    if (instance == NULL) instance = new class_str_init();
    return instance;
}

base_obj *str_upper(base_obj_array args) {
    base_obj *arg0 = args->get(0);
    assert(arg0->get_instance() == class_str::return_instance());
    obj_str *str_obj = (obj_str *)arg0;
    int length = str_obj->get_length();
    if (length <= 0) return parser::obj_none;

    char char_obj;
    char *obj = new char[length];
    for (int i = 0; i < length; i++) {
        char_obj = str_obj->get_value()[i];
        if (char_obj >= 'a' && char_obj <= 'z') obj[i] = char_obj - 0x20;
        else obj[i] = char_obj;
    }
    obj_str *res_obj = new obj_str(obj, length);
    delete[] obj;
    return res_obj;
}

base_obj *str_join(base_obj_array args) {
    obj_str *arg0 = (obj_str *)(args->get(0));
    return arg0->join(args->get(1));
}

base_obj *str_iter_next(base_obj_array args) {
    obj_str_iter *iter = (obj_str_iter *)(args->get(0));
    obj_str *str = iter->get_container();
    int iter_cnt = iter->iter_num();
    if (iter_cnt < str->get_length()) {
        base_obj *obj = new obj_str(&(str->get_value()[iter_cnt]), 1);
        iter->add_iter_num();
        return obj;
    } else return NULL;
}
