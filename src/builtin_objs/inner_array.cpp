#include <new>
#include "vm/parser.hpp"
#include "builtin_objs/inner_array.hpp"
#include "extern_objs/base_obj.hpp"

class base_obj;
class obj_str;
class loop_block;
class base_class;

template<typename T>
inner_array<T>::inner_array(int len, bool flag_short_array) {
    _length = len;
    _size = 0;
    _array = new T[len];
    _flag_short_array = flag_short_array;
}

template<typename T>
int inner_array<T>::length() {
    return _length;
}

template<typename T>
int inner_array<T>::size() {
    return _size;
}

template<typename T>
T *inner_array<T>::value() {
    return _array;
}

template<typename T>
int inner_array<T>::index(T obj) {
    return 0;
}

template<>
int inner_array<base_obj *>::index(base_obj *obj) {
    for (int i = 0; i < _size; i++)
        if (_array[i]->equal(obj) == parser::obj_true)
            return i;
    return -1;
}

template<typename T>
void inner_array<T>::insert(int index, T obj) {
    if (_size >= _length) extend();
    _array[_size++] = NULL;
    for (int i = _size - 1; i > index; i--) _array[i] = _array[i - 1];
    _array[index] = obj;
}

template<typename T>
void inner_array<T>::add(T obj) {
    if (_size >= _length) extend();
    _array[_size++] = obj;
}

template<typename T>
void inner_array<T>::extend() {
    T *new_array = new T[_length * 2];
    for (int i = 0; i < _length; i++) new_array[i] = _array[i];
    delete[] _array;
    _array = new_array;
    _length *= 2;
}

template<typename T>
T inner_array<T>::pop() {
    return _array[--_size];
}

template<typename T>
void inner_array<T>::delete_element(int index) {
    if (index < 0 || index >= _size) return;
    for (int i = index; i < _size - 1; i++) _array[i] = _array[i + 1];
    if (_size > 0) _size--;
}

template<typename T>
T inner_array<T>::get(int index) {
    return _array[index];
}

template<typename T>
void inner_array<T>::set(int index, T obj) {
    if (_size <= index) _size = index + 1;
    while (_size > _length) extend();
    _array[index] = obj;
}

template<typename T>
void *inner_array<T>::operator new(size_t size) {
    return ::operator new(size);
}

template class inner_array<base_obj *>;
template class inner_array<obj_str *>;
template class inner_array<loop_block *>;
template class inner_array<base_class *>;
