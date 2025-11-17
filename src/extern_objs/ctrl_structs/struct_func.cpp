#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "extern_objs/ctrl_structs/struct_func.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_func *class_func::instance = NULL;
class_native_func *class_native_func::instance = NULL;
class_method *class_method::instance = NULL;

class_func::class_func() {
    set_name(new obj_str("function"));
    add_super_list(class_obj::return_instance());
    obj_type *type_obj = new obj_type();
    type_obj->save_instance(this);
}

class_func *class_func::return_instance() {
    if (instance == NULL) instance = new class_func();
    return instance;
}

size_t class_func::size() {
    return sizeof(class_func);
}

void class_func::print(base_obj *obj) {
    printf("<function object: ");
    struct_func *func_obj = static_cast<struct_func *>(obj);
    assert(func_obj && func_obj->get_instance() == (base_class *)this);
    func_obj->get_func_name()->print();
    printf(">");
}

struct_func::struct_func(base_class *class_instance) {
    _func_code = NULL;
    _func_name = NULL;
    _globals = NULL;
    _defaults = NULL;
    _closure = NULL;
    _flags = 0;
    set_instance(class_instance);
}

struct_func::struct_func(struct_native_func native_func_ptr) {
    _func_code = NULL;
    _func_name = NULL;
    _globals = NULL;
    _closure = NULL;
    _flags = 0;
    _native_func = native_func_ptr;
    set_instance(class_native_func::return_instance());
}

struct_func::struct_func(base_obj *code_object, obj_str *qual_name) {
    struct_code *code_obj = (struct_code *) code_object;
    _func_code = code_obj;
    _func_name = code_obj->_module_name;
    _real_name = qual_name;
    _globals = NULL;
    _closure = NULL;
    _flags = code_obj->_flags;
    set_instance(class_func::return_instance());
}

void struct_func::print() {
    printf("<function object: ");
    get_func_name()->print();
    printf(">");
}

obj_str *struct_func::get_func_name() {
    return _func_name;
}

int struct_func::get_flags() {
    return _flags;
}

struct_dict *struct_func::get_globals() {
    return _globals;
}

void struct_func::set_globals(struct_dict *obj) {
    _globals = obj;
}

base_obj_array struct_func::get_defaults() {
    return _defaults;
}

void struct_func::set_defaults(inner_array<base_obj *> *defaults) {
    if (defaults == NULL) {
        _defaults = NULL;
        return;
    }
    _defaults = new inner_array<base_obj *>(defaults->length());
    for (int i = 0; i < defaults->length(); i++) _defaults->set(i, defaults->get(i));
}

struct_list *struct_func::get_closure() {
    return _closure;
}

void struct_func::set_closure(struct_list *obj) {
    _closure = obj;
}

base_obj *struct_func::call(base_obj_array args) {
    return (*_native_func)(args);
}

class_native_func::class_native_func() {
    set_name(new obj_str("native function"));
    add_super_list(class_func::return_instance());
    obj_type *type_obj = new obj_type();
    type_obj->save_instance(this);
}

class_native_func *class_native_func::return_instance() {
    if (instance == NULL) instance = new class_native_func();
    return instance;
}

size_t class_native_func::size() {
    return sizeof(struct_func);
}

class_method::class_method() {
    set_name(new obj_str("method"));
    add_super_list(class_func::return_instance());
    obj_type *type_obj = new obj_type();
    type_obj->save_instance(this);
}

class_method *class_method::return_instance() {
    if (instance == NULL) instance = new class_method();
    return instance;
}

size_t class_method::size() {
    return sizeof(class_method);
}

struct_method::struct_method(struct_func *func) {
    _func = func;
    _container = NULL;
    set_instance(class_method::return_instance());
}

struct_method::struct_method(struct_func *func, base_obj *container) {
    _func = func;
    _container = container;
    set_instance(class_method::return_instance());
}

struct_func *struct_method::get_func() {
    return _func;
}

base_obj *struct_method::get_container() {
    return _container;
}

void struct_method::set_container(base_obj *obj) {
    _container = obj;
}

bool struct_method::is_function(base_obj *obj) {
    base_class *val_instance = obj->get_instance();
    if (val_instance == (base_class *) class_func::return_instance()) return true;
    if (val_instance->mro() == NULL) return false;
    for (int i = 0; i < val_instance->mro()->size(); i++)
        if (val_instance->mro()->get(i) == class_func::return_instance()->get_type_object())
            return true;
    return false;
}

base_obj *len(base_obj_array args) {
    return args->get(0)->len();
}

base_obj *type(base_obj_array args) {
    base_obj *arg0 = args->get(0);
    return arg0->get_instance()->get_type_object();
}

base_obj *isinstance(base_obj_array args) {
    base_obj *obj1 = args->get(0);
    base_obj *obj2 = args->get(1);
    assert(obj2 && obj2->get_instance() == class_type::return_instance());
    base_class *val_instance = obj1->get_instance();
    if (val_instance->get_type_object() == obj2) return parser::obj_true;
    for (int i = 0; i < val_instance->mro()->size(); i++)
        if (val_instance->mro()->get(i) == obj2)
            return parser::obj_true;
    return parser::obj_false;
}

base_obj *iter(base_obj_array args) {
    return args->get(0)->iter();
}

base_obj *next(base_obj_array args) {
    return args->get(0)->next();
}

base_obj *print(base_obj_array args) {
    if (!args || args->length() == 0) return parser::obj_false;
    for (int i = 0; i < args->length(); i++) {
        args->get(i)->print();
        if (i != args->length() - 1) printf(" ");
    }
    printf("\n");
    return parser::obj_true;
}

base_obj *_build_class(base_obj_array args) {
    if (!args || args->length() < 2) return parser::obj_false;
    base_obj *func_obj = args->get(0);
    base_obj *class_name = args->get(1);
    struct_list *bases = new struct_list();
    for (int i = 2; i < args->length(); i++) ((struct_list *) bases)->set(i - 2, args->get(i));
    struct_dict* nspace = NULL;
    base_obj *tmp_obj = executor::return_instance()->call_for_build_class(func_obj, NULL, nspace);
    if (executor::return_instance()->is_exception()) return parser::obj_false;
    base_obj *res = base_class::build_instance(nspace, bases, class_name);
    return res;
}
