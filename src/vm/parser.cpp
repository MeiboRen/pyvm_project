#include <stdio.h>
#include <assert.h>
#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "ctrl_structs/struct_func.hpp"
#include "ctrl_structs/struct_pkg.hpp"
#include "extern_objs/base_obj.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

#define FLAG_REF '\x80'

base_obj *parser::obj_true = NULL;
base_obj *parser::obj_false = NULL;
base_obj *parser::obj_none = NULL;
struct_code *parser::main_code = NULL;
inner_array<base_class *> *parser::array_class = NULL;

// set file pointer and pre-read 256 bytes into buffer
read_pyc::read_pyc(char const* filename) {
    this->filename = filename;
    file_ptr = fopen(filename, "rb");
    fread(buffer, 256 * sizeof(char), 1, file_ptr);
    index = 0;
}

// close file, same as close()
read_pyc::~read_pyc() {
    close();
}

// read a single byte
char read_pyc::read() {
    if (index < 256) return buffer[index++];
    else {
        index = 0;
        fread(buffer, 256 * sizeof(char), 1, file_ptr);
        return buffer[index++];
    }
}

// read a 4-byte integer
int read_pyc::read_int() {
    unsigned int result = 0;
    for (int i = 0; i < 4; i++) {
        // only the lowest 8 bits are retained, and all higher bits are cleared to prevent sign extension issues
        unsigned int byte_val = read() & 0xff;
        result |= byte_val << (i * 8);
    }
    return result;
}

// read an 8-byte double-precision float
double read_pyc::read_float() {
    char dstring[8];
    for (int i = 0; i < 8; i++) dstring[i] = read();
    return *(double*) dstring;
}

// move back a single byte
void read_pyc::unread() {
    index--;
}

// close file
void read_pyc::close() {
    if (file_ptr != NULL) {
        fclose(file_ptr);
        file_ptr = NULL;
    }
}

// return the filename (including the file path)
const char* read_pyc::get_filename() {
    return filename;
}

void parser::initialize() {
    array_class = new inner_array<base_class *>();
    obj_true = new obj_str("True");
    obj_false = new obj_str("False");
    obj_none = new obj_str("None");

    obj_type *type_class = new obj_type();
    base_class *type_instance = class_type::return_instance();
    type_class->save_instance(type_instance);
    type_instance->set_instance_dict(new struct_dict());
    type_instance->set_name(new obj_str("type"));

    obj_type *object_class = new obj_type();
    base_class *object_instance = class_obj::return_instance();
    object_class->save_instance(object_instance);
    object_instance->set_instance_dict(new struct_dict());
    object_instance->set_name(new obj_str("object"));

    type_instance->add_super_list(object_instance);

    class_int::return_instance()->initialize();
    class_str::return_instance()->initialize();
    class_list::return_instance()->initialize();
    class_dict::return_instance()->initialize();
    class_pkg::return_instance()->initialize();
    
    class_int::return_instance()->order_super_list();
    class_str::return_instance()->order_super_list();
    class_list::return_instance()->order_super_list();
    class_dict::return_instance()->order_super_list();
    class_func::return_instance()->order_super_list();
    class_native_func::return_instance()->order_super_list();
    class_method::return_instance()->order_super_list();
    class_pkg::return_instance()->order_super_list();
    type_instance->order_super_list();
    
    executor::return_instance()->initialize();
}

parser::parser(read_pyc *stream) {
    file_stream = stream;
}

struct_code *parser::parse() {
    int magic_number = file_stream->read_int();
    int zero_number = file_stream->read_int();
    int timestamp_number = file_stream->read_int();
    int size_number = file_stream->read_int();
    //printf("magic number: 0x%x\n", magic_number);
    //printf("zero number is 0x%x\n", zero_number);
    //printf("timestamp is 0x%x\n", timestamp_number);
    //printf("file size is 0x%x\n", size_number);

    char object_type = file_stream->read();
    struct_code *result = new struct_code();
    parse_code(result);
    //printf("parse OK!\n");
    return result;
}

obj_str *parser::parse_str(int ref_flag) {
    int length = file_stream->read_int();
    char *str = new char[length];
    for (int i = 0; i < length; i++) {
        str[i] = file_stream->read();
    }

    obj_str *str_obj = new obj_str(str, length);
    delete[] str;
    if(ref_flag) _ref_list.add(str_obj);
    return str_obj;
}

obj_str *parser::parse_short_str(int ref_flag) {
    int length = static_cast<int>(file_stream->read());
    char *str = new char[length];
    for (int i = 0; i < length; i++) {
        str[i] = file_stream->read();
    }

    obj_str *str_obj = new obj_str(str, length);
    delete[] str;
    if(ref_flag) _ref_list.add(str_obj);
    return str_obj;
}

inner_array<base_obj *> *parser::parse_list(int ref_flag) {
    int length = file_stream->read_int();
    inner_array<base_obj *> *list = new inner_array<base_obj *>(length);
    if(ref_flag) {
        _ref_list.add(new struct_list(list));
    }

    for (int i = 0; i < length; i++) {
        char obj_type = file_stream->read();
        int flag = obj_type & FLAG_REF;
        obj_type = obj_type & ~FLAG_REF;
        switch (obj_type) {
            case 'N':
                list->add(parser::obj_none);
                break;
            case 'T': 
                list->add(parser::obj_true);
                break;
            case 'F': 
                list->add(parser::obj_false);
                break;
            case 'c': {
                struct_code *result = new struct_code();
                if(flag) _ref_list.add(result);
                parse_code(result);
                list->add(result);
                break;
            }
            case 'i': {
                obj_int* integer = new obj_int(file_stream->read_int());
                list->add(integer);
                if(flag) _ref_list.add(integer);
                break;
            }
            case 't':
            case 's':
                list->add(parse_str(flag));
                break;
            case 'z':
            case 'Z':
                list->add(parse_short_str(flag));
                break;
            case 'r':
                list->add(_ref_list.get(file_stream->read_int() - 1));
                break;
            case '(':
                //struct_list* tmp_list = ;
                list->add(new struct_list(parse_list(flag)));
                break;
            case ')':
                list->add(new struct_list(parse_short_list(flag)));
                break;
            default:
                printf("Error: unrecognized type %c\n", obj_type);
        }
    }
    return list;
}

inner_array<base_obj *> *parser::parse_short_list(int ref_flag) {
    int length = static_cast<int>(file_stream->read());
    inner_array<base_obj *> *list = new inner_array<base_obj *>(length, true);
    if(ref_flag) {
        _ref_list.add(new struct_list(list));
    }

    for (int i = 0; i < length; i++) {
        char obj_type = file_stream->read();
        int flag = obj_type & FLAG_REF;
        obj_type = obj_type & ~FLAG_REF;
        switch (obj_type) {
            case 'N':
                list->add(parser::obj_none);
                break;
            case 'T': 
                list->add(parser::obj_true);
                break;
            case 'F': 
                list->add(parser::obj_false);
                break;
            case 'c': {
                struct_code *result = new struct_code();
                if(flag) _ref_list.add(result);
                parse_code(result);
                list->add(result);
                break;
            }
            case 'i': {
                obj_int* integer = new obj_int(file_stream->read_int());
                list->add(integer);
                if(flag) _ref_list.add(integer);
                break;
            }
            case 't':
            case 's':
                list->add(parse_str(flag));
                break;
            case 'z':
            case 'Z':
                list->add(parse_short_str(flag));
                break;
            case 'r':
                list->add(_ref_list.get(file_stream->read_int() - 1));
                break;
            case '(':
                list->add(new struct_list(parse_list(flag)));
                break;
            case ')':
                list->add(new struct_list(parse_short_list(flag)));
                break;
            default:
                printf("Error: unrecognized type %c\n", obj_type);
        }
    }
    return list;
}

void parser::parse_code(struct_code* code_obj) {
    int argcount = file_stream->read_int();
    int posonlyargcount = file_stream->read_int();
    int kwonlyargcount = file_stream->read_int();
    int nlocals = file_stream->read_int();
    int stacksize = file_stream->read_int();
    int flags = file_stream->read_int();
    //printf("argcount is 0x%x\n", argcount);
    //printf("posonlyargcount is 0x%x\n", posonlyargcount);
    //printf("kwonlyargcount is 0x%x\n", kwonlyargcount);
    //printf("nlocals is 0x%x\n", nlocals);
    //printf("stacksize is 0x%x\n", stacksize);
    //printf("flags is 0x%x\n", flags);
    
    obj_str *byte_codes = parse_byte_codes();
    inner_array<base_obj *> *consts = parse_consts();
    /*for (int i = 0; i < consts->length(); i++) {
        consts->get(i)->print();
        printf(" ");
    }
    printf("\n");*/
    inner_array<base_obj *> *names = parse_names();
    inner_array<base_obj *> *var_names = parse_var_names();
    inner_array<base_obj *> *free_vars = parse_free_vars();
    inner_array<base_obj *> *cell_vars = parse_cell_vars();

    obj_str *file_name = parse_name();
    obj_str *module_name = parse_name();
    int firstlineno = file_stream->read_int();
    obj_str *lnotab = parse_lnotab();
    
    obj_str *file_path = new obj_str(file_stream->get_filename());
    code_obj->set_code_obj(argcount, posonlyargcount, kwonlyargcount, nlocals, stacksize, flags, firstlineno, byte_codes,
                           consts, names, var_names, free_vars, cell_vars, file_name, module_name, lnotab, file_path);
}

obj_str *parser::parse_byte_codes() {
    char byte_codes_type = file_stream->read();
    int flag = byte_codes_type & FLAG_REF;
    byte_codes_type = byte_codes_type & ~FLAG_REF;

    if (byte_codes_type == 'r') return (obj_str *)(_ref_list.get(file_stream->read_int() - 1));
    else if (byte_codes_type == 's' || byte_codes_type == 't') return parse_str(flag);
    else if (byte_codes_type == 'z' || byte_codes_type == 'Z') return parse_short_str(flag);
    else return NULL;
}

inner_array<base_obj *> *parser::parse_consts() {
    char consts_type = file_stream->read();
    int flag = consts_type & FLAG_REF;
    consts_type = consts_type & ~FLAG_REF;

    if (consts_type == 'r') return ((struct_list *)(_ref_list.get(file_stream->read_int() - 1)))->get_inner_list();
    else if (consts_type == '(') return parse_list(flag);
    else if (consts_type == ')') return parse_short_list(flag);
    else return NULL;
}

inner_array<base_obj *> *parser::parse_names() {
    char names_type = file_stream->read();
    int flag = names_type & FLAG_REF;
    names_type = names_type & ~FLAG_REF;

    if (names_type == 'r') return ((struct_list *)(_ref_list.get(file_stream->read_int() - 1)))->get_inner_list(); 
    else if (names_type == '(') return parse_list(flag);
    else if (names_type == ')') return parse_short_list(flag);
    else return NULL;
}

inner_array<base_obj *> *parser::parse_var_names() {
    char var_names_type = file_stream->read();
    int flag = var_names_type & FLAG_REF;
    var_names_type = var_names_type & ~FLAG_REF;

    if (var_names_type == 'r') return ((struct_list *)(_ref_list.get(file_stream->read_int() - 1)))->get_inner_list();
    else if (var_names_type == '(') return parse_list(flag);
    else if (var_names_type == ')') return parse_short_list(flag);
    else return NULL;
}

inner_array<base_obj *> *parser::parse_free_vars() {
    char free_vars_type = file_stream->read();
    int flag = free_vars_type & FLAG_REF;
    free_vars_type = free_vars_type & ~FLAG_REF;

    if (free_vars_type == 'r') return ((struct_list *)(_ref_list.get(file_stream->read_int() - 1)))->get_inner_list();
    else if (free_vars_type == '(') return parse_list(flag);
    else if (free_vars_type == ')') return parse_short_list(flag);
    else return NULL;
}

inner_array<base_obj *> *parser::parse_cell_vars() {
    char cell_vars_type = file_stream->read();
    int flag = cell_vars_type & FLAG_REF;
    cell_vars_type = cell_vars_type & ~FLAG_REF;

    if (cell_vars_type == 'r') return ((struct_list *)(_ref_list.get(file_stream->read_int() - 1)))->get_inner_list();
    else if (cell_vars_type == '(') return parse_list(flag);
    else if (cell_vars_type == ')') return parse_short_list(flag);
    else return NULL;
}

obj_str *parser::parse_name() {
    char name_type = file_stream->read();
    int flag = name_type & FLAG_REF;
    name_type = name_type & ~FLAG_REF;

    if (name_type == 'r') return (obj_str *)(_ref_list.get(file_stream->read_int() - 1));
    else if (name_type == 's' || name_type == 't') return parse_str(flag);
    else if (name_type == 'z' || name_type == 'Z') return parse_short_str(flag);
    else return NULL;
}

obj_str *parser::parse_lnotab() {
    char lnotab_type = file_stream->read();
    int flag = lnotab_type & FLAG_REF;
    lnotab_type = lnotab_type & ~FLAG_REF;

    if (lnotab_type == 'r') return (obj_str *)(_ref_list.get(file_stream->read_int() - 1));
    else if (lnotab_type == 's' || lnotab_type == 't') return parse_str(flag);
    else if (lnotab_type == 'z' || lnotab_type == 'Z') return parse_short_str(flag);
    else return NULL;
}
