#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "ctrl_structs/struct_func.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_list *class_list::instance = NULL;
class_list_iter *class_list_iter::instance = NULL;

class_list::class_list() {
    return;
}

class_list *class_list::return_instance() {
    if (instance == NULL) instance = new class_list();
    return instance;
}

size_t class_list::size() {
    return sizeof(struct_list);
}

void class_list::initialize() {
    struct_dict *class_instance_dict = new struct_dict();
    class_instance_dict->put(new obj_str("append"), new struct_func(list_append));
    class_instance_dict->put(new obj_str("insert"), new struct_func(list_insert));
    class_instance_dict->put(new obj_str("index"), new struct_func(list_index));
    class_instance_dict->put(new obj_str("pop"), new struct_func(list_pop));
    class_instance_dict->put(new obj_str("pop_left"), new struct_func(list_pop_left));
    class_instance_dict->put(new obj_str("remove"), new struct_func(list_remove));
    class_instance_dict->put(new obj_str("reverse"), new struct_func(list_reverse));
    class_instance_dict->put(new obj_str("sort"), new struct_func(list_sort));
    set_instance_dict(class_instance_dict);
    set_name(new obj_str("list"));
    add_super_list(class_obj::return_instance());
    (new obj_type())->save_instance(this);
}

void class_list::print(base_obj *obj1) {
    struct_list *val1 = (struct_list *)obj1;
    assert(val1 && val1->get_instance() == (base_class *)this);
    printf("[");
    int size = val1->_inner_list->size();
    if (size >= 1) val1->_inner_list->get(0)->print();
    for (int i = 1; i < size; i++) {
        printf(", ");
        val1->_inner_list->get(i)->print();
    }
    printf("]");
}

base_obj *class_list::create_instance(base_obj *callable, inner_array<base_obj *> *args) {
    if (!args || args->length() == 0) return new struct_list();
    if (args->length() == 1) {
        base_obj* arg = args->get(0);
        base_obj* iter_obj = arg->iter();
        if (iter_obj != parser::obj_none) {
            base_obj* item;
            struct_list* new_list = new struct_list();
            while ((item = iter_obj->next()) != NULL) new_list->append(item);
            return new_list;
        }
        else {
            struct_list* new_list = new struct_list();
            new_list->append(arg);
            return new_list;
        }
    } else {
        struct_list* new_list = new struct_list();
        for (int i = 0; i < args->length(); i++) new_list->append(args->get(i));
        return new_list;
    }
}

base_obj *class_list::greater(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) {
        if (base_class::compare_instance(obj1->get_instance(), obj2->get_instance()) < 0) return parser::obj_false;
        else return parser::obj_true;
    }
    struct_list *val1 = (struct_list *)obj1;
    assert(val1 && val1->get_instance() == (base_class *)this);
    struct_list *val2 = (struct_list *)obj2;
    assert(val2 && val2->get_instance() == (base_class *)this);
    int len = val1->size() < val2->size() ? val1->size() : val2->size();
    for (int i = 0; i < len; i++) {
        if (val1->get(i)->less(val2->get(i)) == parser::obj_true) return parser::obj_false;
        else if (val1->get(i)->equal(val2->get(i)) != parser::obj_true) return parser::obj_true;
    }
    if (val1->size() < val2->size()) return parser::obj_false;
    return parser::obj_true;
}

base_obj *class_list::less(base_obj *obj1, base_obj *obj2) {
    if (obj1->get_instance() != obj2->get_instance()) {
        if (base_class::compare_instance(obj1->get_instance(), obj2->get_instance()) < 0) return parser::obj_true;
        else return parser::obj_false;
    }
    struct_list *val1 = (struct_list *)obj1;
    assert(val1 && val1->get_instance() == (base_class *)this);
    struct_list *val2 = (struct_list *)obj2;
    assert(val2 && val2->get_instance() == (base_class *)this);
    int len = val1->size() < val2->size() ? val1->size() : val2->size();
    for (int i = 0; i < len; i++) {
        if (val1->get(i)->less(val2->get(i)) == parser::obj_true) return parser::obj_true;
        else if (val1->get(i)->equal(val2->get(i)) != parser::obj_true) return parser::obj_false;
    }
    if (val1->size() < val2->size()) return parser::obj_true;
    return parser::obj_false;
}

base_obj *class_list::add(base_obj *obj1, base_obj *obj2) {
    struct_list *val1 = (struct_list *)obj1;
    assert(val1 && val1->get_instance() == (base_class *)this);
    struct_list *val2 = (struct_list *)obj2;
    assert(val2 && val2->get_instance() == (base_class *)this);

    struct_list *val3 = new struct_list();
    for (int i = 0; i < val1->size(); i++) val3->get_inner_list()->set(i, val1->get_inner_list()->get(i));
    for (int i = 0; i < val2->size(); i++) val3->get_inner_list()->set(i + val1->size(), val2->get_inner_list()->get(i));
    return val3;
}

base_obj *class_list::mul(base_obj *obj1, base_obj *obj2) {
    struct_list *val1 = (struct_list *)obj1;
    assert(val1 && val1->get_instance() == (base_class *)this);
    obj_int *val2 = (obj_int *)obj2;
    assert(val2 && val2->get_instance() == class_int::return_instance());

    struct_list *val3 = new struct_list();
    for (int i = 0; i < val2->get_value(); i++)
        for (int j = 0; j < val1->size(); j++)
            val3->get_inner_list()->set(i * val1->size() + j, val1->get_inner_list()->get(j));
    return val3;
}

base_obj *class_list::len(base_obj *obj) {
    assert(obj->get_instance() == this);
    return new obj_int(((struct_list *)obj)->size());
}

base_obj *class_list::contains(base_obj *obj1, base_obj *obj2) {
    struct_list *val1 = (struct_list *)obj1;
    assert(val1 && val1->get_instance() == (base_class *)this);
    int size = val1->get_inner_list()->size();
    for (int i = 0; i < size; i++)
        if (val1->get_inner_list()->get(i)->equal(obj2) == parser::obj_true)
            return parser::obj_true;
    return parser::obj_false;
}

base_obj *class_list::subscr(base_obj *obj1, base_obj *obj2) {
    assert(obj1 && obj1->get_instance() == (base_class *)this);
    assert(obj2 && obj2->get_instance() == class_int::return_instance());
    struct_list *val1 = (struct_list *)obj1;
    obj_int *val2 = (obj_int *)obj2;
    return val1->get_inner_list()->get(val2->get_value());
}

void class_list::store_subscr(base_obj *obj1, base_obj *obj2, base_obj *obj3) {
    assert(obj1 && obj1->get_instance() == (base_class *)this);
    assert(obj2 && obj2->get_instance() == class_int::return_instance());
    struct_list *val1 = (struct_list *)obj1;
    obj_int *val2 = (obj_int *)obj2;
    val1->get_inner_list()->set(val2->get_value(), obj3);
}

void class_list::delete_subscr(base_obj *obj1, base_obj *obj2) {
    assert(obj1 && obj1->get_instance() == (base_class *)this);
    assert(obj2 && obj2->get_instance() == class_int::return_instance());
    struct_list *val1 = (struct_list *)obj1;
    obj_int *val2 = (obj_int *)obj2;
    val1->get_inner_list()->delete_element(val2->get_value());
}

base_obj *class_list::iter(base_obj *obj) {
    assert(obj && obj->get_instance() == this);
    return new obj_list_iter((struct_list *)obj);
}

struct_list::struct_list() {
    set_instance(class_list::return_instance());
    _inner_list = new inner_array<base_obj *>();
}

struct_list::struct_list(base_obj_array val_list) {
    set_instance(class_list::return_instance());
    _inner_list = val_list;
}

inner_array<base_obj*>* struct_list::get_inner_list() {
    return _inner_list;
}

int struct_list::size() {
    return _inner_list->size();
}

void struct_list::append(base_obj* obj) {
    _inner_list->add(obj);
}

void struct_list::insert(int index, base_obj* obj) {
    _inner_list->insert(index, obj);
}

base_obj* struct_list::top() {
    return get(size() - 1);
}

base_obj* struct_list::pop() {
    return _inner_list->pop();
}

base_obj* struct_list::get(int index) {
    return _inner_list->get(index);
}

void struct_list::set(int index, base_obj* obj) {
    _inner_list->set(index, obj);
}

int struct_list::index(base_obj *obj) {
    for (int i = 0; i < size(); i++)
        if (get(i) == obj)
            return i;
    return -1;
}

void struct_list::delete_element(int index) {
    _inner_list->delete_element(index);
}

class_list_iter::class_list_iter() {
    struct_dict *class_instance_dict = new struct_dict();
    class_instance_dict->put(new obj_str("__next__"), new struct_func(list_iter_next));
    set_instance_dict(class_instance_dict);
}

class_list_iter *class_list_iter::return_instance() {
    if (instance == NULL) instance = new class_list_iter();
    return instance;
}

obj_list_iter::obj_list_iter(struct_list *container) {
    _container = container;
    _iter_num = 0;
    set_instance(class_list_iter::return_instance());
}

struct_list* obj_list_iter::get_container() {
    return _container;
}

int obj_list_iter::get_iter_num() {
    return _iter_num;
}

void obj_list_iter::add_iter_num() {
    _iter_num++;
}

base_obj *list_append(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    list->append(args->get(1));
    return parser::obj_none;
}

base_obj *list_insert(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    obj_int *index = (obj_int *)args->get(1);
    assert(index && index->get_instance() == class_int::return_instance());
    list->insert(index->get_value(), args->get(2));
    return parser::obj_none;
}

base_obj *list_index(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    base_obj *target = (base_obj *)(args->get(1));
    assert(list && list->get_instance() == class_list::return_instance());
    for (int i = 0; i < list->size(); i++)
        if (list->get(i)->equal(target) == parser::obj_true)
            return new obj_int(i);
    return parser::obj_none;
}

base_obj *list_pop(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    assert(list && list->get_instance() == class_list::return_instance());
    return list->pop();
}

base_obj *list_pop_left(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    assert(list && list->get_instance() == class_list::return_instance());
    base_obj *val = list->get(0);
    list->delete_element(0);
    return val;
}

base_obj *list_remove(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    assert(list && list->get_instance() == class_list::return_instance());
    for (int i = 0; i < list->size(); i++)
        if (list->get(i)->equal(args->get(1)) == parser::obj_true) {
            list->get_inner_list()->delete_element(i);
            break;
        }
            
    return parser::obj_none;
}

base_obj *list_reverse(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    assert(list && list->get_instance() == class_list::return_instance());
    int i = 0, j = list->size() - 1;
    while (i < j) {
        base_obj *val = list->get(i);
        list->set(i, list->get(j));
        list->set(j, val);
        i++;
        j--;
    }
    return parser::obj_none;
}

base_obj *list_sort(base_obj_array args) {
    struct_list *list = (struct_list *)(args->get(0));
    assert(list && list->get_instance() == class_list::return_instance());

    // bubble sort
    int size = list->size();
    for (int i = 0; i < size; i++)
        for (int j = size - 1; j > i; j--)
            if (list->get(j)->less(list->get(j - 1)) == parser::obj_true) {
                base_obj *tmp = list->get(j);
                list->set(j, list->get(j - 1));
                list->set(j - 1, tmp);
            }
    return parser::obj_none;
}

base_obj *list_iter_next(base_obj_array args) {
    obj_list_iter *iter = (obj_list_iter *)(args->get(0));
    int iter_cnt = iter->get_iter_num();
    struct_list *inner_container = iter->get_container();
    if (iter_cnt < inner_container->get_inner_list()->size()) {
        base_obj *obj = inner_container->get(iter_cnt);
        iter->add_iter_num();
        return obj;
    } else return NULL;
}
