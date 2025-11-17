#include <string.h>
#include <math.h>
#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "builtin_objs/inner_dict.hpp"
#include "extern_objs/ctrl_structs/byte_code.hpp"
#include "extern_objs/ctrl_structs/struct_call.hpp"
#include "extern_objs/ctrl_structs/struct_closure.hpp"
#include "extern_objs/ctrl_structs/struct_func.hpp"
#include "extern_objs/ctrl_structs/struct_pkg.hpp"
#include "extern_objs/ctrl_structs/trace.hpp"
#include "extern_objs/data_types/obj_int.hpp"
#include "extern_objs/data_types/obj_float.hpp"
#include "extern_objs/data_types/obj_str.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"
#include "extern_objs/data_structs/struct_set.hpp"

executor *executor::_instance = NULL;

executor::executor() {
    _status_obj = IS_OK;
    _frames = NULL;
}

executor *executor::return_instance() {
    if (_instance == NULL) _instance = new executor();
    return _instance;
}

executor::class_status executor::get_status() {
    return _status_obj;
}

struct_pkg *executor::get_builtins() {
    return _builtins;
}

bool executor::is_exception() {
    return _status_obj == IS_EXCEPTION;
}

void executor::initialize() {
    _builtins = new struct_pkg(new struct_dict());
    _builtins->put(new obj_str("True"), parser::obj_true);
    _builtins->put(new obj_str("False"), parser::obj_false);
    _builtins->put(new obj_str("None"), parser::obj_none);
    _builtins->put(new obj_str("object"), class_obj::return_instance()->get_type_object());
    _builtins->put(new obj_str("int"), class_int::return_instance()->get_type_object());
    _builtins->put(new obj_str("float"), class_float::return_instance()->get_type_object());
    _builtins->put(new obj_str("str"), class_str::return_instance()->get_type_object());
    _builtins->put(new obj_str("list"), class_list::return_instance()->get_type_object());
    _builtins->put(new obj_str("dict"), class_dict::return_instance()->get_type_object());
    _builtins->put(new obj_str("set"), class_set::return_instance()->get_type_object());
    _builtins->put(new obj_str("len"), new struct_func(len));
    _builtins->put(new obj_str("type"), new struct_func(type));
    _builtins->put(new obj_str("isinstance"), new struct_func(isinstance));
    _builtins->put(new obj_str("iter"), new struct_func(iter));
    _builtins->put(new obj_str("next"), new struct_func(next));
    _builtins->put(new obj_str("print"), new struct_func(print));
    _builtins->put(new obj_str("_build_class"), new struct_func(_build_class));
    
    _modules = new struct_dict();
    _builtins->extend(struct_pkg::import_module((obj_str *)parser::obj_none, new obj_str("builtin")));
    _modules->put(new obj_str("__builtins__"), _builtins);
}

void executor::execute(struct_code *codes) {
    _frames = new struct_call(codes);
    _frames->locals()->put(class_str_init::return_instance()->name_str, new obj_str("__main__"));
    execute_frame();
}

void executor::end_execute() {
    if (_status_obj == IS_EXCEPTION) {
        _status_obj = IS_OK;
        _traceback->print();
        ((obj_type *)_exception_obj)->get_own_instance()->get_name()->print();
        printf(": ");
        _exception_info->print();
        printf("\n");

        _traceback = NULL;
        _exception_info = NULL;
        _exception_obj = NULL;
    }
    destroy_frame();
}

struct_dict *executor::execute_module(struct_code *code, obj_str *module_name) {
    struct_call *frame = new struct_call(code);
    frame->set_entry_frame(true);
    frame->locals()->put(class_str_init::return_instance()->name_str, module_name);
    enter_frame(frame);
    execute_frame();
    struct_dict *res = frame->locals();
    destroy_frame();
    return res;
}

void executor::execute_frame() {
    base_obj *obj1, *obj2, *obj3;
    loop_block *loop_obj;
    inner_array<base_obj *> *args = NULL;
    struct_list *stack;

    unsigned char op_code;
    int op_arg = 0;
    while (_frames->is_code_not_ended()) {
        op_code = _frames->get_op_code();
        op_arg = _frames->get_op_arg();
        //printf("STACK LEN: %d\n", _frames->stack()->size());
        //printf("BYTE CODE: %d; %d\n", op_code, op_arg);
        while (op_code == byte_code::EXTENDED_ARG) {
            int extended_arg = op_arg;
            op_code = _frames->get_op_code();
            op_arg = _frames->get_op_arg();
            op_arg = (extended_arg << 8) | op_arg;
        }

        if (op_code == byte_code::POP_TOP) {
            _frames->stack()->pop();
            goto check_status;
        }
        else if (op_code == byte_code::ROT_TWO) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            _frames->stack()->append(obj1);
            _frames->stack()->append(obj2);
            goto check_status;
        }
        else if (op_code == byte_code::ROT_THREE) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            obj3 = _frames->stack()->pop();
            _frames->stack()->append(obj1);
            _frames->stack()->append(obj2);
            _frames->stack()->append(obj3);
            goto check_status;
        }
        else if (op_code == byte_code::DUP_TOP) {
            _frames->stack()->append(_frames->stack()->top());
            goto check_status;
        }
        else if (op_code == byte_code::NOP) {
            goto check_status;
        }
        else if (op_code == byte_code::BINARY_MULTIPLY || op_code == byte_code::INPLACE_MULTIPLY) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            _frames->stack()->append(obj2->mul(obj1));
            goto check_status;
        }
        else if (op_code == byte_code::BINARY_MODULO || op_code == byte_code::INPLACE_MODULO) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            obj3 = obj2->mod(obj1);
            stack = _frames->stack();
            stack->append(obj3);
            goto check_status;
        }
        else if (op_code == byte_code::BINARY_ADD || op_code == byte_code::INPLACE_ADD) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            obj3 = obj2->add(obj1);
            stack = _frames->stack();
            stack->append(obj3);
            goto check_status;
        }
        else if (op_code == byte_code::BINARY_SUBTRACT || op_code == byte_code::INPLACE_SUBTRACT) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            _frames->stack()->append(obj2->sub(obj1));
            goto check_status;
        }
        else if (op_code == byte_code::BINARY_SUBSCR) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            _frames->stack()->append(obj2->subscr(obj1));
            goto check_status;
        }
        else if (op_code == byte_code::BINARY_FLOOR_DIVIDE || op_code == byte_code::INPLACE_FLOOR_DIVIDE) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            if (obj1->get_instance() == class_int::return_instance() && obj2->get_instance() == class_int::return_instance()) {
                int v1 = ((obj_int*)obj2)->get_value();
                int v2 = ((obj_int*)obj1)->get_value();
                if (v2 == 0) {
                    base_obj *zero_division_error = _builtins->get(new obj_str("ZeroDivisionError"));
                    base_obj *error_msg = new obj_str("division by zero");
                    do_raise(zero_division_error, error_msg, NULL);
                } else _frames->stack()->append(new obj_int(v1 / v2));
            } else {
                double v1, v2;
                if (obj2->get_instance() == class_float::return_instance()) v1 = ((obj_float*)obj2)->get_value();
                else v1 = (double)((obj_int*)obj2)->get_value();
                if (obj1->get_instance() == class_float::return_instance()) v2 = ((obj_float*)obj1)->get_value();
                else v2 = (double)((obj_int*)obj1)->get_value();
                if (v2 == 0.0) {
                    base_obj *zero_division_error = _builtins->get(new obj_str("ZeroDivisionError"));
                    base_obj *error_msg = new obj_str("division by zero");
                    do_raise(zero_division_error, error_msg, NULL);
                } else _frames->stack()->append(new obj_float(floor(v1 / v2)));
            }
            goto check_status;
        }
        /*else if (op_code == byte_code::BINARY_TRUE_DIVIDE) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            _frames->stack()->append(obj2->div(obj1));
            goto check_status;
        }*/
        else if (op_code == byte_code::BINARY_TRUE_DIVIDE) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            if (obj1->get_instance() == class_int::return_instance() && 
                obj2->get_instance() == class_int::return_instance()) {
                int v1 = ((obj_int*)obj2)->get_value();
                int v2 = ((obj_int*)obj1)->get_value();
                if (v2 == 0) {
                    base_obj *zero_division_error = _builtins->get(new obj_str("ZeroDivisionError"));
                    base_obj *error_msg = new obj_str("division by zero");
                    do_raise(zero_division_error, error_msg, NULL);
                } else _frames->stack()->append(new obj_float((double)v1 / (double)v2));
            } else {
                double v1, v2;
                if (obj2->get_instance() == class_float::return_instance()) v1 = ((obj_float*)obj2)->get_value();
                else v1 = (double)((obj_int*)obj2)->get_value();
                if (obj1->get_instance() == class_float::return_instance()) v2 = ((obj_float*)obj1)->get_value();
                else v2 = (double)((obj_int*)obj1)->get_value();
                if (v2 == 0.0) {
                    base_obj *zero_division_error = _builtins->get(new obj_str("ZeroDivisionError"));
                    base_obj *error_msg = new obj_str("division by zero");
                    do_raise(zero_division_error, error_msg, NULL);
                } else _frames->stack()->append(new obj_float(v1 / v2));
            }
            goto check_status;
        }
        else if (op_code == byte_code::STORE_SUBSCR) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            obj3 = _frames->stack()->pop();
            obj2->store_subscr(obj1, obj3);
            goto check_status;
        }
        else if (op_code == byte_code::DELETE_SUBSCR) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            obj2->delete_subscr(obj1);
            goto check_status;
        }
        else if (op_code == byte_code::GET_ITER) {
            obj1 = _frames->stack()->pop();
            _frames->stack()->append(obj1->iter());
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_BUILD_CLASS) {
            obj1 = new obj_str("_build_class");
            obj2 = _frames->locals()->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            obj2 = _frames->globals()->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            obj2 = _builtins->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            _frames->stack()->append(parser::obj_none);
            goto check_status;
        }
        else if (op_code == byte_code::RETURN_VALUE) {
            _return_value = _frames->stack()->pop();
            _status_obj = IS_RETURN;
            goto check_status;
        }
        else if (op_code == byte_code::POP_BLOCK) {
            loop_obj = _frames->_loop_stack->pop();
            while (_frames->stack()->size() > loop_obj->_level) _frames->stack()->pop();
            goto check_status;
        }
        else if (op_code == byte_code::POP_EXCEPT) {
            if (_frames->loop_stack()->size() > 0) {
                loop_obj = _frames->loop_stack()->pop();
                while (_frames->stack()->size() > loop_obj->_level) _frames->stack()->pop();
                if (_status_obj == IS_EXCEPTION) {
                    _exception_obj = NULL;
                    _exception_info = NULL;
                    _traceback = NULL;
                    _status_obj = IS_OK;
                }
                delete loop_obj;
            }
            goto check_status;
        }
        else if (op_code == byte_code::STORE_NAME) {
            obj1 = _frames->names()->get(op_arg);
            _frames->locals()->put(obj1, _frames->stack()->pop());
            goto check_status;
        }
        else if (op_code == byte_code::DELETE_NAME) {
            base_obj* name_obj = _frames->names()->get(op_arg);
            if (_frames->locals()->has_key(name_obj)) _frames->locals()->remove(name_obj);
            else if (_frames->globals()->has_key(name_obj)) _frames->globals()->remove(name_obj);
            else {
                base_obj* error_msg = name_obj->add(new obj_str(" is not defined"));
                base_obj* name_error = _builtins->get(new obj_str("NameError"));
                if (name_error == parser::obj_none) {
                    printf("NameError: ");
                    name_obj->print();
                    printf(" is not defined\n");
                    _status_obj = IS_EXCEPTION;
                } else do_raise(name_error, error_msg, NULL);
            }
            goto check_status;
        }
        else if (op_code == byte_code::UNPACK_SEQUENCE) {
            obj1 = _frames->stack()->pop();
            while (op_arg--) _frames->stack()->append(obj1->subscr(new obj_int(op_arg)));
            goto check_status;
        }
        else if (op_code == byte_code::FOR_ITER) {
            obj1 = _frames->stack()->top();
            obj2 = obj1->next();
            if (obj2 == NULL) {
                _frames->_pc += op_arg * 2;
                _status_obj = IS_OK;
                _exception_info = NULL;
            } else _frames->stack()->append(obj2);
            goto check_status;
        }
        else if (op_code == byte_code::STORE_ATTR) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->names()->get(op_arg);
            obj3 = _frames->stack()->pop();
            obj1->set_attr(obj2, obj3);
            goto check_status;
        }
        else if (op_code == byte_code::STORE_GLOBAL) {
            obj1 = _frames->names()->get(op_arg);
            _frames->globals()->put(obj1, _frames->stack()->pop());
            goto check_status;
        }
        else if (op_code == byte_code::ROT_N) {
            for (int i = 0; i < op_arg; i++) {
                int index = _frames->stack()->size() - op_arg;
                _frames->stack()->append(_frames->stack()->get(index));
            }
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_CONST) {
            obj1 = _frames->consts()->get(op_arg);
            _frames->stack()->append(obj1);
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_NAME) {
            obj1 = _frames->names()->get(op_arg);
            obj2 = _frames->locals()->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            obj2 = _frames->globals()->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            obj2 = _builtins->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            _frames->stack()->append(parser::obj_none);
            goto check_status;
        }
        else if (op_code == byte_code::BUILD_TUPLE || op_code == byte_code::BUILD_LIST) {
            obj1 = new struct_list();
            while (op_arg--) {
                obj2 = _frames->stack()->pop();
                ((struct_list *)obj1)->set(op_arg, obj2);
            }
            _frames->stack()->append(obj1);
            goto check_status;
        }
        else if (op_code == byte_code::BUILD_SET) {
            struct_set *set_obj = new struct_set();
            while (op_arg--) {
                base_obj *item = _frames->stack()->pop();
                set_obj->add(item);
            }
            _frames->stack()->append(set_obj);
            goto check_status;
        }
        else if (op_code == byte_code::BUILD_MAP) {
            obj1 = new struct_dict();
            for(int i = 0;i < op_arg ;i++) {
                obj2 = _frames->stack()->pop();
                obj3 = _frames->stack()->pop();
                ((struct_dict *)obj1)->put(obj3, obj2);
            }
            _frames->stack()->append(obj1);
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_ATTR || op_code == byte_code::LOAD_METHOD) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->_names->get(op_arg);
            _frames->stack()->append(obj1->get_attr(obj2));
            goto check_status;
        }
        else if (op_code == byte_code::COMPARE_OP) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            switch (op_arg) {
                case byte_code::GREATER:
                    _frames->stack()->append(obj2->greater(obj1));
                    break;
                case byte_code::LESS:
                    _frames->stack()->append(obj2->less(obj1));
                    break;
                case byte_code::EQUAL:
                    _frames->stack()->append(obj2->equal(obj1));
                    break;
                case byte_code::NOT_EQUAL:
                    _frames->stack()->append(obj2->not_equal(obj1));
                    break;
                case byte_code::GREATER_EQUAL:
                    _frames->stack()->append(obj2->ge(obj1));
                    break;
                case byte_code::LESS_EQUAL:
                    _frames->stack()->append(obj2->le(obj1));
                    break;
                case byte_code::EXC_MATCH: {
                    bool found = false;
                    base_class *obj_class = ((obj_type *)obj2)->get_own_instance();
                    if (obj1 != obj2) {
                        for (int i = 0; i < obj_class->mro()->size(); i++)
                            if (obj2->get_instance()->mro()->get(i) == obj1) {
                                found = true;
                                break;
                            }
                    } else found = true;
                    if (found) _frames->stack()->append(parser::obj_true);
                    else _frames->stack()->append(parser::obj_false);
                    break;
                }
                default:
                    printf("Error: Unrecognized compare op %d\n", op_arg);
            }
            goto check_status;
        }
        else if (op_code == byte_code::IMPORT_NAME) {
            _frames->stack()->pop();
            _frames->stack()->pop();
            obj1 = _frames->names()->get(op_arg);
            obj2 = _modules->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            obj_str *cursor_code_path;
            if (_frames->_codes->_file_path != NULL) cursor_code_path = _frames->_codes->_file_path;
            else cursor_code_path = parser::main_code->_file_path;
            obj2 = struct_pkg::import_module(cursor_code_path, obj1);
            if (obj2 == NULL) {
                base_obj *module_not_found_error = _builtins->get(new obj_str("ModuleNotFoundError"));
                obj_str *error_prefix = new obj_str("No module named '");
                obj_str *error_suffix = new obj_str("'");
                obj_str *error_msg_obj = (obj_str *)error_prefix->add(obj1)->add(error_suffix);
                executor::return_instance()->do_raise(module_not_found_error, error_msg_obj, NULL);
                goto check_status;
            }
            _modules->put(obj1, obj2);
            _frames->stack()->append(obj2);
            goto check_status;
        }
        else if (op_code == byte_code::IMPORT_FROM) {
            obj1 = _frames->names()->get(op_arg);
            obj2 = _frames->stack()->top();
            obj3 = ((struct_pkg *)obj2)->get(obj1);
            _frames->stack()->append(obj3);
            goto check_status;
        }
        else if (op_code == byte_code::JUMP_FORWARD) {
            _frames->set_pc(op_arg * 2 + _frames->get_pc());
            goto check_status;
        }
        else if (op_code == byte_code::JUMP_ABSOLUTE) {
            _frames->set_pc(op_arg * 2);
            goto check_status;
        }
        else if (op_code == byte_code::POP_JUMP_IF_FALSE) {
            obj1 = _frames->stack()->pop();
            if (obj1 == parser::obj_false) _frames->set_pc(op_arg * 2);
            goto check_status;
        }
        else if (op_code == byte_code::POP_JUMP_IF_TRUE) {
            obj1 = _frames->stack()->pop();
            if (obj1 == parser::obj_true) _frames->set_pc(op_arg * 2);
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_GLOBAL) {
            obj1 = _frames->names()->get(op_arg);
            obj2 = _frames->globals()->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            obj2 = _builtins->get(obj1);
            if (obj2 != parser::obj_none) {
                _frames->stack()->append(obj2);
                goto check_status;
            }
            _frames->stack()->append(parser::obj_none);
            goto check_status;
        }
        else if (op_code == byte_code::IS_OP) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            switch (op_arg) {
                case 0:
                    if (obj1 == obj2) _frames->stack()->append(parser::obj_true);
                    else _frames->stack()->append(parser::obj_false);
                    break;
                case 1:
                    if (obj1 == obj2) _frames->stack()->append(parser::obj_false);
                    else _frames->stack()->append(parser::obj_true);
                    break;
                default:
                    printf("Error: Unrecognized compare op %d\n", op_arg);
            }
            goto check_status;
        }
        else if (op_code == byte_code::CONTAINS_OP) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            switch (op_arg) {
                case 0:
                    _frames->stack()->append(obj1->contains(obj2));
                    break;
                case 1:
                    obj3 = obj1->contains(obj2);
                    if (obj3 == parser::obj_false) _frames->stack()->append(parser::obj_true);
                    else _frames->stack()->append(parser::obj_false);
                    break;
                default:
                    printf("Error: Unrecognized compare op %d\n", op_arg);
            }
            goto check_status;
        }
        else if (op_code == byte_code::JUMP_IF_NOT_EXC_MATCH) {
            base_obj* exc_class = _frames->stack()->pop();
            base_obj* exc_instance = _frames->stack()->pop();
            bool match = false;
            base_class *obj_class = ((obj_type *)exc_instance)->get_own_instance();
            if (exc_instance != exc_class) {
                for (int i = 0; i < obj_class->mro()->size(); i++)
                    if (obj_class->mro()->get(i) == exc_class) {
                        match = true;
                        goto check_status;
                    }
            } else match = true;
            if (!match) _frames->set_pc(op_arg * 2);
            goto check_status;
        }
        else if (op_code == byte_code::SETUP_FINALLY) {
            _frames->loop_stack()->add(new loop_block(op_code, _frames->get_pc() + op_arg * 2, _frames->stack()->size()));
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_FAST) {
            _frames->stack()->append(_frames->fast_locals()->get(op_arg));
            goto check_status;
        }
        else if (op_code == byte_code::STORE_FAST) {
            _frames->fast_locals()->set(op_arg, _frames->stack()->pop());
            goto check_status;
        }
        else if (op_code == byte_code::GEN_START) {
            _frames->stack()->pop();
            goto check_status;
        }
        else if (op_code == byte_code::RAISE_VARARGS) {
            obj1 = obj2 = obj3 = NULL;
            switch (op_arg) {
                case 3:
                    obj3 = _frames->stack()->pop();
                case 2:
                    obj2 = _frames->stack()->pop();
                case 1:
                    obj1 = _frames->stack()->pop();
                    break;
            }
            do_raise(obj1, obj2, obj3);
            goto check_status;
        }
        else if (op_code == byte_code::CALL_FUNCTION || op_code == byte_code::CALL_METHOD) {
            if (op_arg > 0) {
                int arg_num = op_arg & 0xff;
                int key_arg_num = op_arg >> 8;
                int arg_cnt = arg_num + 2 * key_arg_num;
                args = new inner_array<base_obj *>(arg_cnt);
                while (arg_cnt--) args->set(arg_cnt, _frames->stack()->pop());
                /*for (int i = 0; i < args->length(); i++) {
                    args->get(i)->print();
                    printf(" ");
                }
                printf("\n");*/
            }
            obj1 = _frames->stack()->pop();
            // temp methed to solve the testcase like, i = dict(1, 2)
            if (obj1->get_instance() == class_type::return_instance()) {
                obj_type* type_obj = (obj_type*)obj1;
                if (type_obj->get_own_instance() == class_dict::return_instance()) {
                    if (args && args->length() > 1) {
                        int key_arg_num = op_arg >> 8;
                        if (key_arg_num == 0) {
                            char buffer[128];
                            snprintf(buffer, sizeof(buffer), "%s%d", "dict expected at most 1 argument, got ", args->length());
                            base_obj* error_msg = new obj_str(buffer);
                            do_raise(_builtins->get(new obj_str("TypeError")), error_msg, NULL);
                            if (args != NULL) args = NULL;
                            goto check_status;
                        }
                    }
                }
            }
            build_frame(obj1, args, op_arg);
            if (args != NULL) args = NULL;
            goto check_status;
        }
        else if (op_code == byte_code::MAKE_FUNCTION) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->pop();
            struct_func *func_obj = new struct_func(obj2, (obj_str*)obj1);
            func_obj->set_globals(_frames->globals());
            if (op_arg & 0x01) {
                struct_list *l = (struct_list *)_frames->stack()->pop();
                args = new inner_array<base_obj *>(l->size());
                for (int i = 0; i < l->size(); i++) args->set(i, l->get(i));
            }
            if (op_arg & 0x08) {
                base_obj* closure = _frames->stack()->pop();
                if (closure->get_instance() == class_list::return_instance()) func_obj->set_closure((struct_list*)closure);
            }
            func_obj->set_defaults(args);
            if (args != NULL) args = NULL;
            _frames->stack()->append(func_obj);
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_CLOSURE) {
            obj1 = _frames->closure()->get(op_arg);
            if (obj1 == NULL) {
                obj1 = _frames->get_cell_obj(op_arg);
                _frames->closure()->set(op_arg, obj1);
            }
            if (obj1->get_instance() == class_closure::return_instance()) _frames->stack()->append(obj1);
            else _frames->stack()->append(new struct_closure(_frames->closure(), op_arg));
            goto check_status;
        }
        else if (op_code == byte_code::LOAD_DEREF) {
            obj1 = _frames->closure()->get(op_arg);
            if (obj1->get_instance() == class_closure::return_instance()) obj1 = ((struct_closure *)obj1)->value();
            _frames->stack()->append(obj1);
            goto check_status;
        }
        else if (op_code == byte_code::STORE_DEREF) {
            _frames->closure()->set(op_arg, _frames->stack()->pop());
            goto check_status;
        }
        else if (op_code == byte_code::CALL_FUNCTION_KW) {
            obj1 = _frames->stack()->pop();
            if (op_arg > 0 && obj1->get_instance() == class_list::return_instance()) {
                int arg_num = op_arg >> 8;
                int key_arg_num = op_arg & 0xff;
                int arg_cnt = arg_num + 2 * key_arg_num;
                args = new inner_array<base_obj *>(arg_cnt);
                for(int i = op_arg - 1; i >= 0 ; i--) {
                    obj2 = _frames->stack()->pop();
                    args->set(2 * i + 1, obj2);
                    args->set(2 * i, ((struct_list *)obj1)->get(i));
                }
                op_arg = arg_num | (key_arg_num << 8);
            }
            build_frame(_frames->stack()->pop(), args, op_arg);
            if (args != NULL) args = NULL;
            goto check_status;
        }
        else if (op_code == byte_code::FORMAT_VALUE) {
            obj1 = _frames->stack()->pop();
            int which_conversion = op_arg & 0x4;
            int has_format = (op_arg & 0x4) == 0x4;
            if (has_format) obj2 = _frames->stack()->pop();
            base_obj* str_obj = obj1->get_instance()->to_str(obj1);
            _frames->stack()->append(str_obj);
            goto check_status;
        }
        else if (op_code == byte_code::BUILD_CONST_KEY_MAP) {
            obj1 = new struct_dict();
            obj2 = _frames->stack()->pop();
            if (obj2->get_instance() == class_list::return_instance()) {
                for(int i = op_arg - 1; i >= 0 ; i--) {
                    obj3 = _frames->stack()->pop();
                    ((struct_dict *)obj1)->put(((struct_list *)obj2)->get(i), obj3);
                }
                _frames->stack()->append(obj1);
            }
            goto check_status;
        }
        else if (op_code == byte_code::BUILD_STRING) {
            obj_str** strings = new obj_str*[op_arg];
            int total_length = 0;
            for (int i = 0; i < op_arg; i++) {
                base_obj *str_obj = _frames->stack()->pop();
                if (str_obj->get_instance() == class_str::return_instance()) {
                    strings[op_arg - 1 - i] = (obj_str *)str_obj;
                    total_length += strings[op_arg - 1 - i]->get_length();
                }
            }
            int pos = 0;
            char* result_buffer = new char[total_length + 1];
            for (int i = 0; i < op_arg; i++) {
                obj_str* str = strings[i];
                memcpy(result_buffer + pos, str->get_value(), str->get_length());
                pos += str->get_length();
            }
            result_buffer[total_length] = '\0';
            obj_str* result_str = new obj_str(result_buffer);
            delete[] result_buffer;
            delete[] strings;
            _frames->stack()->append(result_str);
            goto check_status;
        }
        else if (op_code == byte_code::LIST_EXTEND) {
            struct_list *l = (struct_list *)_frames->stack()->pop();
            int index = _frames->stack()->size() - op_arg;
            struct_list *peek_list = (struct_list *)_frames->stack()->get(index);
            for(int i = 0;i < l->size();i++) peek_list->append(l->get(i));
            goto check_status;
        }
        /*else if (op_code == byte_code::SET_UPDATE) {
            obj1 = _frames->stack()->pop();
            obj2 = _frames->stack()->get(_frames->stack()->size() - 1 - op_arg);
            if (obj2->get_instance() == class_set::return_instance()) {
                ((struct_set *)obj2)->update(obj1);
            }
            goto check_status;
        }*/
        else if (op_code == byte_code::SET_UPDATE) {
            if (_frames->stack()->size() < 2) goto check_status;
            int set_index = _frames->stack()->size() - 1 - op_arg;
            base_obj *set_obj = _frames->stack()->get(set_index);
            base_obj *iterable = _frames->stack()->pop();
            if (set_obj->get_instance() == class_set::return_instance()) {
                if (iterable->get_instance() == class_set::return_instance()) {
                    struct_set *other_set = (struct_set *)iterable;
                    for (int i = 0; i < other_set->size(); i++) {
                        base_obj *item = other_set->get_inner_set()->get(i);
                        ((struct_set *)set_obj)->add(item);
                    }
                } else {
                    base_obj *iter_obj = iterable->iter();
                    if (iter_obj && iter_obj != parser::obj_none) {
                        base_obj *item;
                        while ((item = iter_obj->next()) != NULL) ((struct_set *)set_obj)->add(item);
                    }
                }
            } else printf("Error: SET_UPDATE expected set object\n");
            goto check_status;
        }
        else printf("Error: Unrecognized byte code %d\n", op_code);
        
        check_status:
        while (_status_obj != IS_OK && _frames->loop_stack()->size() != 0) {
            loop_obj = _frames->_loop_stack->pop();
            while (_frames->stack()->size() > loop_obj->_level) _frames->stack()->pop();

            if (loop_obj->_type == byte_code::SETUP_FINALLY) {
                if (_status_obj == IS_EXCEPTION) {
                    _frames->stack()->append(_traceback);
                    _frames->stack()->append(_exception_info);
                    _frames->stack()->append(_exception_obj);
                    _traceback = NULL;
                    _exception_info = NULL;
                    _exception_obj = NULL;
                } else {
                    if (_status_obj == IS_RETURN || _status_obj == IS_CONTINUE) _frames->stack()->append(_return_value);
                    // use the last bit to indicate that it is a direct integer and not a pointer
                    _frames->stack()->append((base_obj*)(((intptr_t)_status_obj << 1) | 0x1));
                }
                _frames->_pc = loop_obj->_target;
                _status_obj = IS_OK;
            }
        }
        if (_status_obj != IS_OK && _frames->_loop_stack->size() == 0) {
            if (_status_obj == IS_EXCEPTION) {
                _return_value = NULL;
                ((struct_tb *)_traceback)->record_frame(_frames);
            }
            if (_status_obj == IS_RETURN) _status_obj = IS_OK;
            if (_frames->is_first_frame() || _frames->is_entry_frame()) return;
            leave_frame();
        }
    }
}

void executor::destroy_frame() {
    struct_call *temp = _frames;
    _frames = _frames->get_saved_frame();
    delete temp;
}

void executor::build_frame(base_obj *callable, base_obj_array args, int op_arg) {
    if (callable->get_instance() == class_native_func::return_instance()) _frames->stack()->append(((struct_func *)callable)->call(args));
    else if (callable->get_instance() == class_method::return_instance()) {
        struct_method *method = (struct_method *)callable;
        if (!args) args = new inner_array<base_obj *>(1);
        args->insert(0, method->get_container());
        build_frame(method->get_func(), args, op_arg + 1);
    } else if (callable->get_instance() == class_func::return_instance()) {
        struct_call *frame = new struct_call((struct_func *)callable, args, op_arg);
        frame->set_saved_frame(_frames);
        _frames = frame;
    } else if (callable->get_instance() == class_type::return_instance()) {
        base_obj *instance = ((obj_type *)callable)->get_own_instance()->create_instance(callable, args);
        _frames->stack()->append(instance);
    } else {
        base_obj *method = callable->get_attr(class_str_init::return_instance()->call_str);
        if (method != parser::obj_none) build_frame(method, args, op_arg);
        else {
            printf("Error: can not call a normal function/method ");
            callable->print();
            printf("\n");
        }
    }
}

void executor::leave_frame() {
    destroy_frame();
    _frames->stack()->append(_return_value);
}

void executor::enter_frame(struct_call *frame) {
    frame->set_saved_frame(_frames);
    _frames = frame;
}

base_obj *executor::call_virtual(base_obj *func, base_obj_array args) {
    if (func->get_instance() == class_native_func::return_instance()) return ((struct_func *)func)->call(args);
    else if (func->get_instance() == class_method::return_instance()) {
        struct_method *method = (struct_method *)func;
        if (!args) args = new inner_array<base_obj *>(1);
        args->insert(0, method->get_container());
        return call_virtual(method->get_func(), args);
    } else if (struct_method::is_function(func)) {
        int size = 0;
        if (args) size = args->size();
        struct_call *frame = new struct_call((struct_func *)func, args, size);
        frame->set_entry_frame(true);
        enter_frame(frame);
        execute_frame();
        destroy_frame();
        return _return_value;
    }
    return parser::obj_none;
}

base_obj *executor::call_for_build_class(base_obj *func, base_obj_array args, struct_dict*& res_locals) {
    if (func->get_instance() == class_native_func::return_instance()) {
        res_locals = NULL;
        return ((struct_func *)func)->call(args);
    } else if (func->get_instance() == class_method::return_instance()) {
        struct_method *method = (struct_method *)func;
        if (!args) args = new inner_array<base_obj *>(1);
        args->insert(0, method->get_container());
        return call_for_build_class(method->get_func(), args, res_locals);
    } else if (struct_method::is_function(func)) {
        int size = 0;
        if (args) size = args->size();
        struct_call *frame = new struct_call((struct_func *)func, args, size);
        frame->set_entry_frame(true);
        enter_frame(frame);
        execute_frame();
        res_locals = frame->locals();
        destroy_frame();
        return _return_value;
    }
    res_locals = NULL;
    return parser::obj_none;
}

executor::class_status executor::do_raise(base_obj *exc, base_obj *val, base_obj *traceback_obj) {
    assert(exc != NULL);
    _status_obj = IS_EXCEPTION;
    if (traceback_obj == NULL) traceback_obj = new struct_tb();
    if (val != NULL) {
        _exception_obj = exc;
        _exception_info = val;
        _traceback = traceback_obj;
        return IS_EXCEPTION;
    }
    if (exc->get_instance() == class_type::return_instance()) {
        _exception_info = call_virtual(exc, NULL);
        _exception_obj = exc;
    } else {
        _exception_info = exc;
        _exception_obj = _exception_info->get_instance()->get_type_object();
    }
    _traceback = traceback_obj;
    return IS_EXCEPTION;
}
