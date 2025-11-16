#include <assert.h>
#include "vm/parser.hpp"
#include "ctrl_structs/struct_func.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_dict *class_dict::instance = NULL;
template<iter_type T>
class_dict_iter<T> *class_dict_iter<T>::instance = NULL;

class_dict::class_dict() {
    return;
}

class_dict *class_dict::return_instance() {
    if (instance == NULL) instance = new class_dict();
    return instance;
}

size_t class_dict::size() {
    return sizeof(struct_dict);
}

void class_dict::initialize() {
    struct_dict *class_instance_dict = new struct_dict();
    class_instance_dict->put(new obj_str("setdefault"), new struct_func(dict_set_default));
    class_instance_dict->put(new obj_str("remove"), new struct_func(dict_remove));
    class_instance_dict->put(new obj_str("pop"), new struct_func(dict_pop));
    class_instance_dict->put(new obj_str("keys"), new struct_func(dict_keys));
    class_instance_dict->put(new obj_str("values"), new struct_func(dict_values));
    class_instance_dict->put(new obj_str("items"), new struct_func(dict_items));
    set_instance_dict(class_instance_dict);
    set_name(new obj_str("dict"));
    add_super_list(class_obj::return_instance());
    (new obj_type())->save_instance(this);
}

void class_dict::print(base_obj *obj) {
    struct_dict *dict_obj = (struct_dict *) obj;
    assert(dict_obj && dict_obj->get_instance() == (base_class *)this);
    printf("{");
    int size = dict_obj->_map->size();
    if (size >= 1) {
        dict_obj->_map->entries()[0]._k->print();
        printf(": ");
        dict_obj->_map->entries()[0]._v->print();
    }
    for (int i = 1; i < size; i++) {
        printf(", ");
        dict_obj->_map->entries()[i]._k->print();
        printf(": ");
        dict_obj->_map->entries()[i]._v->print();
    }
    printf("}");
}

base_obj *class_dict::create_instance(base_obj *callable, inner_array<base_obj *> *args) {
    if (!args || args->length() == 0) return new struct_dict();
    if (args->length() == 1) {
        base_obj* arg = args->get(0);
        if (arg->get_instance() == class_dict::return_instance()) {
            struct_dict* dict_arg = (struct_dict*)arg;
            struct_dict* new_dict = new struct_dict();
            new_dict->update(dict_arg);
            return new_dict;
        }
        else {
            base_obj* iter_obj = arg->iter();
            if (iter_obj != parser::obj_none) {
                struct_dict* new_dict = new struct_dict();
                base_obj* item;
                while ((item = iter_obj->next()) != NULL)
                    if (item->get_instance() == class_list::return_instance()) {
                        struct_list* pair = (struct_list*)item;
                        if (pair->size() == 2) new_dict->put(pair->get(0), pair->get(1));
                    }
                return new_dict;
            }
        }
        return new struct_dict();
    }
    struct_dict* new_dict = new struct_dict();
    if (args->length() % 2 == 0) {
        for (int i = 0; i < args->length(); i += 2) {
            base_obj* key = args->get(i);
            base_obj* value = args->get(i + 1);
            new_dict->put(key, value);
        }
        return new_dict;
    }
    return new_dict;
}

base_obj *class_dict::subscr(base_obj* obj1, base_obj* obj2) {
    assert(obj1 && obj1->get_instance() == (base_class *)this);
    return ((struct_dict *)obj1)->get_map()->get(obj2);
}

void class_dict::store_subscr(base_obj* obj1, base_obj* obj2, base_obj *obj3) {
    assert(obj1 && obj1->get_instance() == (base_class *)this);
    ((struct_dict *)obj1)->put(obj2, obj3);
}

void class_dict::delete_subscr(base_obj* obj1, base_obj* obj2) {
    assert(obj1 && obj1->get_instance() == (base_class *)this);
    ((struct_dict *)obj1)->remove(obj2);
}

base_obj *class_dict::iter(base_obj *obj) {
    base_obj *val = new obj_dict_iter((struct_dict *)obj);
    val->set_instance(class_dict_iter<ITER_KEY>::return_instance());
    return val;
}

struct_dict::struct_dict() {
    _map = new inner_dict<base_obj *, base_obj *>();
    set_instance(class_dict::return_instance());
}

struct_dict::struct_dict(inner_dict<base_obj *, base_obj *> *obj) {
    _map = obj;
    set_instance(class_dict::return_instance());
}

inner_dict<base_obj*, base_obj*>* struct_dict::get_map() {
    return _map;
}

int struct_dict::size() {
    return _map->size();
}

bool struct_dict::has_key(base_obj* key) {
    return _map->has_key(key);
}

base_obj* struct_dict::get(base_obj* key) {
    return _map->get(key);
}

void struct_dict::put(base_obj* key, base_obj* val) {
    _map->put(key, val);
}

base_obj* struct_dict::remove(base_obj* key) {
    return _map->remove(key);
}

void struct_dict::update(struct_dict *dict) {
    for (int i = 0; i < dict->size(); i++) {
        put(dict->get_map()->get_key(i), dict->get_map()->get_value(i));
    }
}

template<iter_type T>
class_dict_iter<T>::class_dict_iter() {
    const char *class_instance_names[] = {
        "dictionary-keyiterator",
        "dictionary-valueiterator",
        "dictionary-itemiterator",
    };
    set_name(new obj_str(class_instance_names[T]));
    struct_dict *class_instance_dict = new struct_dict();
    class_instance_dict->put(new obj_str("__next__"), new struct_func(dict_iter_next));
    set_instance_dict(class_instance_dict);
}

template<iter_type T>
class_dict_iter<T> *class_dict_iter<T>::return_instance() {
    if (instance == NULL) instance = new class_dict_iter<T>();
    return instance;
}

template<iter_type T>
base_obj *class_dict_iter<T>::iter(base_obj *obj) {
    return obj;
}

template<iter_type T>
base_obj *class_dict_iter<T>::next(base_obj *obj) {
    obj_dict_iter *iter = (obj_dict_iter *) obj;
    int iter_num = iter->get_iter_num();
    struct_dict *iter_dict = iter->get_container();
    if (iter_num >= iter_dict->get_map()->size()) return NULL;
    if (iter_num < iter_dict->get_map()->size()) {
        base_obj *obj;
        if (T == ITER_KEY) obj = iter_dict->get_map()->get_key(iter_num);
        else if (T == ITER_VALUE) obj = iter_dict->get_map()->get_value(iter_num);
        else if (T == ITER_ITEM) {
            struct_list *val = new struct_list();
            val->append(iter_dict->get_map()->get_key(iter_num));
            val->append(iter_dict->get_map()->get_value(iter_num));
            obj = val;
        }
        iter->add_iter_num();
        return obj;
    } else return NULL;
}

obj_dict_iter::obj_dict_iter(struct_dict *dict) {
    _container = dict;
    _iter_num = 0;
}

struct_dict *obj_dict_iter::get_container() {
    return _container;
}

int obj_dict_iter::get_iter_num() {
    return _iter_num;
}

void obj_dict_iter::add_iter_num() {
    _iter_num++;
}

base_obj *dict_set_default(base_obj_array args) {
    struct_dict *dict = (struct_dict *)(args->get(0));
    base_obj *key = args->get(1);
    base_obj *value = args->get(2);
    if (!dict->has_key(key)) dict->put(key, value);
    return parser::obj_none;
}

base_obj *dict_remove(base_obj_array args) {
    base_obj *obj1 = args->get(0);
    base_obj *obj2 = args->get(1);
    return ((struct_dict *) obj1)->remove(obj2);
}

base_obj *dict_pop(base_obj_array args) {
    base_obj *obj1 = args->get(0);
    base_obj *obj2 = args->get(1);
    if (((struct_dict *) obj1)->has_key(obj2)) ((struct_dict *) obj1)->remove(obj2);
    return parser::obj_none;
}

base_obj *dict_keys(base_obj_array args) {
    struct_dict *obj = (struct_dict *)(args->get(0));
    struct_list *keys = new struct_list();
    for (int i = 0; i < obj->size(); i++) keys->append(obj->get_map()->get_key(i));
    return keys;
}

base_obj *dict_values(base_obj_array args) {
    struct_dict *obj = (struct_dict *)(args->get(0));
    struct_list *values = new struct_list();
    for (int i = 0; i < obj->size(); i++) values->append(obj->get_map()->get_value(i));
    return values;
}

base_obj *dict_items(base_obj_array args) {
    struct_dict *obj = (struct_dict *)(args->get(0));
    struct_list *items = new struct_list();
    for (int i = 0; i < obj->size(); i++) {
        struct_list *item = new struct_list();
        item->append(obj->get_map()->get_key(i));
        item->append(obj->get_map()->get_value(i));
        items->append(item);
    }
    return items;
}

base_obj *dict_iter_next(base_obj_array args) {
    obj_dict_iter *iter = (obj_dict_iter *)(args->get(0));
    return iter->get_instance()->next(iter);
}

template class class_dict_iter<ITER_KEY>;
template class class_dict_iter<ITER_VALUE>; 
template class class_dict_iter<ITER_ITEM>;
