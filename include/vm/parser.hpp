#pragma once
#include <stdio.h>
#include "builtin_objs/inner_array.hpp"
#include "extern_objs/ctrl_structs/struct_code.hpp"
#include "extern_objs/data_types/obj_str.hpp"

class base_class;
class base_obj;
class obj_int;
class struct_code;
template<typename T>
class inner_array;

class read_pyc{
private:
    FILE *file_ptr;
    char buffer[256];
    const char *filename;
    unsigned short index;
public:
    read_pyc(char const* filename); // set file pointer and pre-read 256 bytes into buffer
    ~read_pyc(); // close file, same as close()
    char read(); // read a single byte
    int read_int(); // read a 4-byte integer
    double read_float(); // read an 8-byte double-precision float
    void unread(); // move back a single byte
    void close(); // close file
    const char* get_filename(); // return the filename (including the file path)
};

class parser {
private:
    read_pyc *file_stream;
    inner_array<base_obj *> _ref_list;
public:
    static base_obj *obj_true, *obj_false, *obj_none;
    static inner_array<base_class *> *array_class;
    static struct_code *main_code;

    parser(read_pyc *stream);
    static void initialize();
    struct_code *parse();
    obj_str *parse_str(int ref_flag);
    obj_str *parse_short_str(int ref_flag);
    inner_array<base_obj *> *parse_list(int ref_flag);
    inner_array<base_obj *> *parse_short_list(int ref_flag);
    void parse_code(struct_code* c);
    obj_str *parse_byte_codes();
    inner_array<base_obj *> *parse_consts();
    inner_array<base_obj *> *parse_names();
    inner_array<base_obj *> *parse_var_names();
    inner_array<base_obj *> *parse_free_vars();
    inner_array<base_obj *> *parse_cell_vars();
    obj_str *parse_name();
    obj_str *parse_lnotab();
};
