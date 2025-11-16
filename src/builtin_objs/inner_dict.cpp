#include "vm/parser.hpp"
#include "builtin_objs/inner_dict.hpp"
#include "extern_objs/base_obj.hpp"

template<typename K, typename V>
inner_kvp<K, V>::inner_kvp() {
    _k = 0;
    _v = 0;
}

template<typename K, typename V>
inner_kvp<K, V>::inner_kvp(K val_k, V val_v) {
    _k = val_k;
    _v = val_v;
}

template<typename K, typename V>
inner_kvp<K, V>::inner_kvp(const inner_kvp<K, V> &entry) {
    _k = entry._k;
    _v = entry._v;
}

template<typename K, typename V>
void *inner_kvp<K, V>::operator new[](size_t size) {
    return ::operator new(size);
}

template<typename K, typename V>
inner_dict<K, V>::inner_dict() {
    _length = 1;
    _size = 0;
    _entries = new inner_kvp<K, V>[_length];
}

template<typename K, typename V>
int inner_dict<K, V>::size() {
    return _size;
}

template<typename K, typename V>
inner_kvp<K, V> *inner_dict<K, V>::entries() {
    return _entries;
}

template<typename K, typename V>
int inner_dict<K, V>::index(K val_key) {
    for (int i = 0; i < _size; i++)
        if (_entries[i]._k->equal(val_key) == (base_obj *)parser::obj_true)
            return i;
    return -1;
}

template<typename K, typename V>
void inner_dict<K, V>::put(K val_key, V val_val) {
    for (int i = 0; i < _size; i++)
        if (_entries[i]._k->equal(val_key) == (base_obj *)parser::obj_true) {
            _entries[i]._v = val_val;
            return;
        }
    extend();
    _entries[_size++] = inner_kvp<K, V>(val_key, val_val);
}

template<typename K, typename V>
void inner_dict<K, V>::extend() {
    if (_size >= _length) {
        inner_kvp<K, V> *new_entries = new inner_kvp<K, V>[_length * 2];
        for (int i = 0; i < _size; i++) new_entries[i] = _entries[i];
        _length *= 2;
        delete[] _entries;
        _entries = new_entries;
    }
}

template<typename K, typename V>
V inner_dict<K, V>::remove(K val_key) {
    int i = index(val_key);
    if (i < 0) return 0;
    V val_val = _entries[i]._v;
    _entries[i] = _entries[--_size];
    return val_val;
}

template<typename K, typename V>
bool inner_dict<K, V>::has_key(K val_key) {
    if (index(val_key) >= 0) return true;
    else return false;
}

template<typename K, typename V>
V inner_dict<K, V>::get(K val_key) {
    int i = index(val_key);
    if (i < 0) return parser::obj_none;
    else return _entries[i]._v;
}

template<typename K, typename V>
K inner_dict<K, V>::get_key(int index) {
    return _entries[index]._k;
}

template<typename K, typename V>
V inner_dict<K, V>::get_value(int index) {
    return _entries[index]._v;
}

template<typename K, typename V>
void *inner_dict<K, V>::operator new(size_t size) {
    return ::operator new(size);
}

template class inner_dict<base_obj *, base_obj *>;
