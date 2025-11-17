#pragma once
#include "builtin_objs/inner_array.hpp"
#include "builtin_objs/inner_dict.hpp"
#include "extern_objs/ctrl_structs/struct_code.hpp"
#include "extern_objs/ctrl_structs/struct_func.hpp"
#include "extern_objs/data_structs/struct_list.hpp"

class loop_block {
public:
    unsigned char _type;
    unsigned int _target, _level;
    loop_block();
    loop_block(unsigned char val_type, unsigned int val_target, int val_level);
    loop_block(const loop_block &obj);
};

class struct_call {
public:
    int _pc;
    bool _entry_frame;
    struct_code *_codes;
    inner_array<base_obj *> *_consts;
    inner_array<base_obj *> *_names;
    struct_dict *_locals;
    struct_dict *_globals;
    struct_list *_fast_locals;
    struct_list *_closure;
    struct_call *_saved_frame;
    struct_list *_stack;
    inner_array<loop_block *> *_loop_stack;

    struct_call(struct_code *codes);
    struct_call(struct_func *func, base_obj_array args, int op_arg);
    void set_pc(int pc);
    int get_pc();
    void set_saved_frame(struct_call *func);
    struct_call *get_saved_frame();
    bool is_first_frame();
    void set_entry_frame(bool flag);
    bool is_entry_frame();
    inner_array<base_obj *> *consts();
    inner_array<base_obj *> *names();
    struct_dict *locals();
    struct_dict *globals();
    struct_list *fast_locals();
    struct_list *closure();
    struct_list *stack();
    inner_array<loop_block *> *loop_stack();
    unsigned char get_op_code();
    int get_op_arg();
    obj_str *file_name();
    obj_str *func_name();
    int firstlineno();
    int current_lineno();
    int calculate_lineno_from_lnotab();
    bool is_code_not_ended();
    base_obj *get_cell_obj(int index);
};
