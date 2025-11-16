#pragma once
#include <stdio.h>

class base_obj;

template<typename T>
class inner_array {
private:
    int _length, _size;
    T *_array;
    bool _flag_short_array;
public:
    inner_array(int len = 1, bool flag_short_array = false);
    int length();
    int size();
    T *value();
    int index(T obj);
    void insert(int index, T obj);
    void add(T obj);
    void extend();
    T pop();
    void delete_element(int index);
    T get(int index);
    void set(int index, T obj);
    void *operator new(size_t size);
};

typedef inner_array<base_obj *> *base_obj_array;
