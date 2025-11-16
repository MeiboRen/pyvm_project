#include "ctrl_structs/struct_call.hpp"
#include "extern_objs/base_obj.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

loop_block::loop_block() {
    _type = 0;
    _target = 0;
    _level = 0;
}

loop_block::loop_block(unsigned char val_type, unsigned int val_target, int val_level) {
    _type = val_type;
    _target = val_target;
    _level = val_level;
}

loop_block::loop_block(const loop_block &obj) {
    _type = obj._type;
    _target = obj._target;
    _level = obj._level;
}

struct_call::struct_call(struct_code *codes) {
    _codes = codes;
    _consts = codes->_consts;
    _names = codes->_names;
    _locals = new struct_dict();
    _globals = _locals;
    _fast_locals = NULL;
    _closure = NULL;
    _stack = new struct_list();
    _loop_stack = new inner_array<loop_block *>();
    _saved_frame = NULL;
    _pc = 0;
    _entry_frame = false;
}

struct_call::struct_call(struct_func *func, base_obj_array args, int op_arg) {
    assert((args && op_arg != 0) || (args == NULL && op_arg == 0));
    _codes = func->_func_code;
    _consts = _codes->_consts;
    _names = _codes->_names;
    _locals = new struct_dict();
    _globals = func->_globals;
    _fast_locals = new struct_list();
    _closure = NULL;

    // formal parameter
    const int val_argcount = _codes->_argcount;
    const int val_arg = op_arg & 0xff;
    const int val_flag = op_arg >> 8;
    int keyword_pos = val_argcount;
    struct_list *arg_list = NULL;
    struct_dict *arg_dict = NULL;
    if (func->_defaults) {
        int arg_num = val_argcount;
        int default_num = func->_defaults->length();
        while (default_num--) _fast_locals->set(--arg_num, func->_defaults->get(default_num));
    }

    // variadic positional argument
    if (val_argcount < val_arg) {
        int i = 0;
        for (; i < val_argcount; i++) _fast_locals->set(i, args->get(i));
        arg_list = new struct_list();
        for (; i < val_arg; i++) arg_list->append(args->get(i));
    } else for (int i = 0; i < val_arg; i++) _fast_locals->set(i, args->get(i));

    // keyword parameter
    for (int i = 0; i < val_flag; i++) {
        base_obj *key = args->get(val_arg + i * 2);
        base_obj *val = args->get(val_arg + i * 2 + 1);
        int index = _codes->_var_names->index(key);
        if (index < 0) {
            if (arg_dict == NULL) arg_dict = new struct_dict();
            arg_dict->put(key, val);
        }
        else _fast_locals->set(index, val);
    }

    // further processing of variadic positional argument
    if (_codes->_flags & 0x4) {
        if (arg_list == NULL) arg_list = new struct_list();
        _fast_locals->set(val_argcount, arg_list);
        keyword_pos += 1;
    } else  if (arg_list != NULL) assert(false);

    // further processing of keyword parameter
    if (_codes->_flags & 0x8) {
        if (arg_dict == NULL) arg_dict = new struct_dict();
        _fast_locals->set(keyword_pos, arg_dict);
    } else if (arg_dict != NULL) assert(false);

    // closure
    inner_array<base_obj *> *cells = _codes->_cell_vars;
    if (cells && cells->size() > 0) {
        _closure = new struct_list();
        for (int i = 0; i < cells->size(); i++) _closure->append(NULL);
    }
    if (func->get_closure() && func->get_closure()->size() > 0) {
        if (_closure == NULL) _closure = func->get_closure();
        else _closure = (struct_list *)_closure->add(func->get_closure());
    }

    _stack = new struct_list();
    _loop_stack = new inner_array<loop_block *>();
    _saved_frame = NULL;
    _pc = 0;
    _entry_frame = false;
}

void struct_call::set_pc(int pc) {
    _pc = pc;
}

int struct_call::get_pc() {
    return _pc;
}

void struct_call::set_saved_frame(struct_call *func) {
    _saved_frame = func;
}

struct_call *struct_call::get_saved_frame() {
    return _saved_frame;
}

bool struct_call::is_first_frame() {
    return _saved_frame == NULL;
}

void struct_call::set_entry_frame(bool flag) {
    _entry_frame = flag;
}

bool struct_call::is_entry_frame() {
    return _entry_frame;
}

inner_array<base_obj *> *struct_call::consts() {
    return _consts;
}

inner_array<base_obj *> *struct_call::names() {
    return _names;
}

struct_dict *struct_call::locals() {
    return _locals;
}

struct_dict *struct_call::globals() {
    return _globals;
}

struct_list *struct_call::fast_locals() {
    return _fast_locals;
}

struct_list *struct_call::closure() {
    return _closure;
}

struct_list *struct_call::stack() {
    return _stack;
}

inner_array<loop_block *> *struct_call::loop_stack() {
    return _loop_stack;
}

unsigned char struct_call::get_op_code() {
    return _codes->_bytecodes->get_value()[_pc++];
}

int struct_call::get_op_arg() {
    int res = _codes->_bytecodes->get_value()[_pc++] & 0xff;
    return res;
}

obj_str *struct_call::file_name() {
    return _codes->_file_name;
}

obj_str *struct_call::func_name() {
    return _codes->_module_name;
}

int struct_call::firstlineno() {
    return _codes->_firstlineno;
}

int struct_call::current_lineno() {
    if (_codes->_lnotab == NULL || _codes->_lnotab->get_length() == 0) return _codes->_firstlineno;
    return calculate_lineno_from_lnotab();
}

int struct_call::calculate_lineno_from_lnotab() {
    int lineno = _codes->_firstlineno;
    int addr = 0;
    unsigned char* lnotab = (unsigned char*)_codes->_lnotab->get_value();
    int lnotab_len = _codes->_lnotab->get_length();
    for (int i = 0; i < lnotab_len; i += 2) {
        addr += lnotab[i];
        lineno += (signed char)lnotab[i + 1];
        if (addr > _pc) break;
    }
    return lineno;
}

bool struct_call::is_code_ended() {
    return _pc < _codes->_bytecodes->get_length();
}

base_obj *struct_call::get_cell_obj(int index) {
    base_obj *cell_name = _codes->_cell_vars->get(index);
    index = _codes->_var_names->index(cell_name);
    return _fast_locals->get(index);
}
