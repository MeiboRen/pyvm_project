#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "extern_objs/ctrl_structs/struct_func.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"
#include "extern_objs/data_structs/struct_set.hpp"

class_set *class_set::instance = NULL;

class_set::class_set() {
    return;
}

class_set *class_set::return_instance() {
    if (instance == NULL) instance = new class_set();
    return instance;
}

size_t class_set::size() {
    return sizeof(struct_set);
}

void class_set::initialize() {
    struct_dict *class_instance_dict = new struct_dict();
    class_instance_dict->put(new obj_str("add"), new struct_func(set_add));
    class_instance_dict->put(new obj_str("remove"), new struct_func(set_remove));
    class_instance_dict->put(new obj_str("pop"), new struct_func(set_pop));
    class_instance_dict->put(new obj_str("clear"), new struct_func(set_clear));
    class_instance_dict->put(new obj_str("contains"), new struct_func(set_contains));
    class_instance_dict->put(new obj_str("union"), new struct_func(set_union));
    class_instance_dict->put(new obj_str("intersection"), new struct_func(set_intersection));
    class_instance_dict->put(new obj_str("difference"), new struct_func(set_difference));
    set_instance_dict(class_instance_dict);
    set_name(new obj_str("set"));
    add_super_list(class_obj::return_instance());
    (new obj_type())->save_instance(this);
}

void class_set::print(base_obj *obj) {
    struct_set *set_obj = (struct_set *)obj;
    assert(set_obj && set_obj->get_instance() == (base_class *)this);
    printf("{");
    int size = set_obj->size();
    if (size >= 1) set_obj->get_inner_set()->get(0)->print();
    for (int i = 1; i < size; i++) {
        printf(", ");
        set_obj->get_inner_set()->get(i)->print();
    }
    printf("}");
}

base_obj *class_set::create_instance(base_obj *callable, inner_array<base_obj *> *args) {
    if (!args || args->length() == 0) return new struct_set();
    if (args->length() == 1) {
        base_obj* arg = args->get(0);
        base_obj* iter_obj = arg->iter();
        if (iter_obj != parser::obj_none) {
            base_obj* item;
            struct_set* new_set = new struct_set();
            while ((item = iter_obj->next()) != NULL) new_set->add(item);
            return new_set;
        }
        else {
            struct_set* new_set = new struct_set();
            new_set->add(arg);
            return new_set;
        }
    } else {
        struct_set* new_set = new struct_set();
        for (int i = 0; i < args->length(); i++) new_set->add(args->get(i));
        return new_set;
    }
}

base_obj *class_set::equal(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) return parser::obj_false;
    struct_set *val1 = (struct_set *)obj1;
    struct_set *val2 = (struct_set *)obj2;
    if (val1->size() != val2->size()) return parser::obj_false;
    for (int i = 0; i < val1->size(); i++) {
        if (!val2->contains(val1->get_inner_set()->get(i))) return parser::obj_false;
    }
    return parser::obj_true;
}

base_obj *class_set::not_equal(base_obj *obj1, base_obj *obj2) {
    base_obj* eq_result = equal(obj1, obj2);
    if (eq_result == parser::obj_true) return parser::obj_false;
    else return parser::obj_true;
}

base_obj *class_set::len(base_obj *obj) {
    assert(obj->get_instance() == this);
    return new obj_int(((struct_set *)obj)->size());
}

base_obj *class_set::contains(base_obj *obj1, base_obj *obj2) {
    struct_set *val1 = (struct_set *)obj1;
    if (val1->contains(obj2)) return parser::obj_true;
    return parser::obj_false;
}

base_obj *class_set::iter(base_obj *obj) {
    assert(obj && obj->get_instance() == this);
    struct_list *list_copy = new struct_list();
    struct_set *set_obj = (struct_set *)obj;
    for (int i = 0; i < set_obj->size(); i++) {
        list_copy->append(set_obj->get_inner_set()->get(i));
    }
    return list_copy->iter();
}

base_obj *class_set::to_str(base_obj *obj) {
    return obj;
}

struct_set::struct_set() {
    set_instance(class_set::return_instance());
    _inner_set = new inner_array<base_obj *>();
}

struct_set::struct_set(inner_array<base_obj *> *val_set) {
    set_instance(class_set::return_instance());
    _inner_set = val_set;
}

inner_array<base_obj*>* struct_set::get_inner_set() {
    return _inner_set;
}

int struct_set::size() {
    return _inner_set->size();
}

void struct_set::add(base_obj* obj) {
    if (!contains(obj)) {
        _inner_set->add(obj);
    }
}

void struct_set::remove(base_obj* obj) {
    int index = _inner_set->index(obj);
    if (index >= 0) {
        _inner_set->delete_element(index);
    }
}

bool struct_set::contains(base_obj* obj) {
    for (int i = 0; i < _inner_set->size(); i++) {
        if (_inner_set->get(i)->equal(obj) == parser::obj_true) return true;
    }
    return false;
}

base_obj* struct_set::pop() {
    if (_inner_set->size() > 0) {
        return _inner_set->pop();
    }
    return parser::obj_none;
}

void struct_set::update(base_obj* iterable) {
    base_obj* iter_obj = iterable->iter();
    base_obj* item;
    while ((item = iter_obj->next()) != NULL) {
        add(item);
    }
}

void struct_set::clear() {
    _inner_set = new inner_array<base_obj *>();
}

struct_set* struct_set::union_with(struct_set* other) {
    struct_set* result = new struct_set();
    for (int i = 0; i < this->size(); i++) {
        result->add(this->_inner_set->get(i));
    }
    for (int i = 0; i < other->size(); i++) {
        result->add(other->_inner_set->get(i));
    }
    return result;
}

struct_set* struct_set::intersection(struct_set* other) {
    struct_set* result = new struct_set();
    for (int i = 0; i < this->size(); i++) {
        base_obj* item = this->_inner_set->get(i);
        if (other->contains(item)) {
            result->add(item);
        }
    }
    return result;
}

struct_set* struct_set::difference(struct_set* other) {
    struct_set* result = new struct_set();
    for (int i = 0; i < this->size(); i++) {
        base_obj* item = this->_inner_set->get(i);
        if (!other->contains(item)) {
            result->add(item);
        }
    }
    return result;
}

base_obj *set_add(base_obj_array args) {
    struct_set *set = (struct_set *)(args->get(0));
    set->add(args->get(1));
    return parser::obj_none;
}

base_obj *set_remove(base_obj_array args) {
    struct_set *set = (struct_set *)(args->get(0));
    base_obj *item = args->get(1);
    if (set->contains(item)) {
        set->remove(item);
    }
    return parser::obj_none;
}

base_obj *set_pop(base_obj_array args) {
    struct_set *set = (struct_set *)(args->get(0));
    return set->pop();
}

base_obj *set_clear(base_obj_array args) {
    struct_set *set = (struct_set *)(args->get(0));
    set->clear();
    return parser::obj_none;
}

base_obj *set_update(base_obj_array args) {
    struct_set *set = (struct_set *)(args->get(0));
    set->update(args->get(1));
    return parser::obj_none;
}

base_obj *set_contains(base_obj_array args) {
    struct_set *set = (struct_set *)(args->get(0));
    base_obj *item = args->get(1);
    if (set->contains(item)) return parser::obj_true;
    else return parser::obj_false;
}

base_obj *set_union(base_obj_array args) {
    struct_set *set1 = (struct_set *)(args->get(0));
    struct_set *set2 = (struct_set *)(args->get(1));
    if (set1->get_instance() != class_set::return_instance() || set2->get_instance() != class_set::return_instance()) {
        printf("Error: set_union requires set objects\n");
        return parser::obj_none;
    }
    return set1->union_with(set2);
}

base_obj *set_intersection(base_obj_array args) {
    struct_set *set1 = (struct_set *)(args->get(0));
    struct_set *set2 = (struct_set *)(args->get(1));
    if (set1->get_instance() != class_set::return_instance() || set2->get_instance() != class_set::return_instance()) {
        printf("Error: set_intersection requires set objects\n");
        return parser::obj_none;
    }
    return set1->intersection(set2);
}

base_obj *set_difference(base_obj_array args) {
    struct_set *set1 = (struct_set *)(args->get(0));
    struct_set *set2 = (struct_set *)(args->get(1));
    if (set1->get_instance() != class_set::return_instance() || set2->get_instance() != class_set::return_instance()) {
        printf("Error: set_difference requires set objects\n");
        return parser::obj_none;
    }
    return set1->difference(set2);
}
