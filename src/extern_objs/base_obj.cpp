#include "vm/parser.hpp"
#include "ctrl_structs/struct_func.hpp"
#include "extern_objs/base_obj.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_obj *class_obj::instance = NULL;
class_type *class_type::instance = NULL;

class_obj::class_obj() {
    return;
}

class_obj *class_obj::return_instance() {
    if (instance == NULL) instance = new class_obj();
    return instance;
}

base_class *base_obj::get_instance() {
    assert(_instance != NULL);
    return _instance;
}

void base_obj::set_instance(base_class *obj) {
    _instance = obj;
}

size_t base_obj::size() {
    return get_instance()->size();
}

void base_obj::print() {
    get_instance()->print(this);
}

struct_dict *base_obj::get_obj_dict() {
    return _obj_dict;
}

void base_obj::set_obj_dict(struct_dict *obj) {
    _obj_dict = obj;
}

void base_obj::init_obj_dict() {
    _obj_dict = new struct_dict();
}

base_obj *base_obj::greater(base_obj *obj) {
    return get_instance()->greater(this, obj);
}

base_obj *base_obj::less(base_obj *obj) {
    return get_instance()->less(this, obj);
}

base_obj *base_obj::equal(base_obj *obj) {
    return get_instance()->equal(this, obj);
}

base_obj *base_obj::not_equal(base_obj *obj) {
    return get_instance()->not_equal(this, obj);
}

base_obj *base_obj::ge(base_obj *obj) {
    return get_instance()->ge(this, obj);
}

base_obj *base_obj::le(base_obj *obj) {
    return get_instance()->le(this, obj);
}

base_obj *base_obj::add(base_obj *obj) {
    return get_instance()->add(this, obj);
}

base_obj *base_obj::sub(base_obj *obj) {
    return get_instance()->sub(this, obj);
}

base_obj *base_obj::mul(base_obj *obj) {
    return get_instance()->mul(this, obj);
}

base_obj *base_obj::div(base_obj *obj) {
    return get_instance()->div(this, obj);
}

base_obj *base_obj::mod(base_obj *obj) {
    return get_instance()->mod(this, obj);
}

base_obj *base_obj::len() {
    return get_instance()->len(this);
}

base_obj *base_obj::contains(base_obj *obj) {
    return get_instance()->contains(this, obj);
}

base_obj *base_obj::subscr(base_obj *obj) {
    return get_instance()->subscr(this, obj);
}

void base_obj::store_subscr(base_obj *obj1, base_obj *obj2) {
    return get_instance()->store_subscr(this, obj1, obj2);
}

void base_obj::delete_subscr(base_obj *obj) {
    return get_instance()->delete_subscr(this, obj);
}

base_obj *base_obj::get_attr(base_obj *obj) {
    return get_instance()->get_attr(this, obj);
}

base_obj *base_obj::set_attr(base_obj *obj1, base_obj *obj2) {
    return get_instance()->set_attr(this, obj1, obj2);
}

base_obj *base_obj::next() {
    return get_instance()->next(this);
}

base_obj *base_obj::iter() {
    return get_instance()->iter(this);
}

void *base_obj::operator new(size_t size) {
    return ::operator new(size);
}

class_type::class_type() {
    return;
}

class_type *class_type::return_instance() {
    if (instance == NULL) instance = new class_type();
    return instance;
}

size_t class_type::size() {
    return sizeof(obj_type);
}

void class_type::print(base_obj *obj) {
    assert(obj->get_instance() == (base_class *)this);
    printf("<class '");
    base_class *instance_obj = ((obj_type *)obj)->get_own_instance();
    struct_dict *attr_dict = instance_obj->get_instance_dict();
    if (attr_dict) {
        base_obj *mod_obj = attr_dict->get((base_obj *)class_str_init::return_instance()->mod_str);
        if (mod_obj != parser::obj_none) {
            mod_obj->print();
            printf(".");
        }
    }
    instance_obj->get_name()->print();
    printf("'>");
}

base_obj *class_type::set_attr(base_obj *obj1, base_obj *obj2, base_obj *obj3) {
    obj_type *type_obj = (obj_type *)obj1;
    type_obj->get_own_instance()->get_instance_dict()->put(obj2, obj3);
    return parser::obj_none;
}

obj_type::obj_type() {
    set_instance(class_type::return_instance());
}

base_class *obj_type::get_own_instance() {
    return _own_instance;
}

void obj_type::save_instance(base_class *val_class) {
    _own_instance = val_class;
    val_class->set_type_object(this);
}
