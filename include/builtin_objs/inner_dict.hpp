#pragma once
#include <stdio.h>

template<typename K, typename V>
class inner_kvp {
public:
    K _k;
    V _v;
    inner_kvp();
    inner_kvp(K k, V v);
    inner_kvp(const inner_kvp<K, V> &entry);
    void *operator new[](size_t size);
};

template<typename K, typename V>
class inner_dict {
private:
    int _length, _size;
    inner_kvp<K, V> *_entries;
public:
    inner_dict();
    int size();
    inner_kvp<K, V> *entries();
    int index(K val_key);
    void put(K val_key, V val_val);
    void extend();
    V remove(K val_key);
    bool has_key(K val_key);
    V get(K val_key);
    K get_key(int index);
    V get_value(int index);
    void *operator new(size_t size);
};
