#pragma once
#include <stdint.h>
#include "extern_objs/ctrl_structs/byte_code.hpp"
#include "extern_objs/ctrl_structs/struct_code.hpp"
#include "extern_objs/ctrl_structs/struct_pkg.hpp"

class struct_call;
class struct_dict;

class executor {
    enum class_status {
        IS_OK,
        IS_BREAK,
        IS_CONTINUE,
        IS_EXCEPTION,
        IS_RETURN
    };
private:
    class_status _status_obj;
    struct_pkg *_builtins;
    struct_dict *_modules;
    struct_call *_frames;
    base_obj *_return_value;
    base_obj *_exception_obj, *_exception_info, *_traceback;
    
    static executor *_instance;
public:
    executor();
    static executor *return_instance();
    class_status get_status();
    struct_pkg *get_builtins();
    bool is_exception();
    void initialize();
    void execute(struct_code *codes);
    void end_execute();
    struct_dict *execute_module(struct_code *codes, obj_str *module_name);
    void execute_frame();
    void destroy_frame();
    void build_frame(base_obj *pObject, base_obj_array args, int op_arg);
    void leave_frame();
    void enter_frame(struct_call *frame);
    base_obj *call_virtual(base_obj *func, base_obj_array args);
    base_obj *call_for_build_class(base_obj *func, base_obj_array args, struct_dict*& res_locals);
    class_status do_raise(base_obj *exc, base_obj *val, base_obj *tb);
};
