#include <string>
#include <dlfcn.h>
#include <unistd.h>
#include "vm/parser.hpp"
#include "vm/executor.hpp"
#include "extern_objs/ctrl_structs/struct_pkg.hpp"
#include "extern_objs/data_structs/struct_list.hpp"
#include "extern_objs/data_structs/struct_dict.hpp"

class_pkg *class_pkg::_instance = NULL;

class_pkg::class_pkg() {
    return;
}

class_pkg *class_pkg::return_instance() {
    if (_instance == NULL) _instance = new class_pkg();
    return _instance;
}

size_t class_pkg::size() {
    return sizeof(struct_pkg);
}

void class_pkg::initialize() {
    set_instance_dict(new struct_dict());
    set_name(new obj_str("module"));
    add_super_list(class_obj::return_instance());
    (new obj_type())->save_instance(this);
}

struct_pkg::struct_pkg(struct_dict *obj) {
    set_obj_dict(obj);
    set_instance(class_pkg::return_instance());
}

base_obj *struct_pkg::get(base_obj *obj) {
    return get_obj_dict()->get(obj);
}

void struct_pkg::put(base_obj *obj1, base_obj *obj2) {
    get_obj_dict()->put(obj1, obj2);
}

void struct_pkg::extend(struct_pkg *module_obj) {
    if (get_obj_dict() == NULL || module_obj->get_obj_dict() == NULL) return;
    get_obj_dict()->update(module_obj->get_obj_dict());
}

struct_pkg *struct_pkg::import_module(obj_str *cursor_path, base_obj *obj) {
    obj_str *module_name = (obj_str *)obj;
    struct_list *so_list = new struct_list();
    so_list->append(class_str_init::return_instance()->libdir_pre_str);
    so_list->append(module_name);
    so_list->append(class_str_init::return_instance()->so_suf_str);
    obj_str *file_name = class_str_init::return_instance()->empty_str->join(so_list);

    // find .so
    if (access(file_name->get_value(), R_OK) == 0) return import_so(module_name);
    file_name = (obj_str *)(module_name->add(class_str_init::return_instance()->pyc_suf_str));

    // find from ./
    std::string path_str, cursor_path_str = cursor_path->get_value();
    const size_t index_last_slash = cursor_path_str.rfind('/');
    if (std::string::npos != index_last_slash) path_str = cursor_path_str.substr(0, index_last_slash);
    obj_str *file_path = new obj_str((path_str + "/" + file_name->get_value()).c_str());

    // find from /lib
    if (access(file_path->get_value(), R_OK) == -1) {
        struct_list *pyc_list = new struct_list();
        pyc_list->append(class_str_init::return_instance()->libdir_pre_str);
        pyc_list->append(module_name);
        pyc_list->append(class_str_init::return_instance()->pyc_suf_str);
        file_path = class_str_init::return_instance()->empty_str->join(pyc_list);
    }
    if (access(file_path->get_value(), R_OK) != 0) return NULL;
    //assert(access(file_path->get_value(), R_OK) == 0);
    read_pyc stream(file_path->get_value());
    parser parser(&stream);
    struct_code *module_code = parser.parse();
    struct_dict *module_dict = executor::return_instance()->execute_module(module_code, module_name);
    return new struct_pkg(module_dict);
}

struct_pkg *struct_pkg::import_so(obj_str *module_name) {
    char *error_msg = NULL;
    obj_str *prefix = class_str_init::return_instance()->libdir_pre_str;
    obj_str *so_suffix = class_str_init::return_instance()->so_suf_str;
    obj_str *file_name = (obj_str *) (prefix->add(module_name)->add(so_suffix));
    void *handle = dlopen(file_name->get_value(), RTLD_NOW);
    if (handle == NULL) {
        printf("Error: can not open file %s\n", dlerror());
        return NULL;
    }
    obj_str *method_prefix = new obj_str("init_");
    obj_str *init_method = (obj_str *)(method_prefix->add(module_name));
    struct_func_init init_func = (struct_func_init)dlsym(handle, init_method->get_value());
    if ((error_msg = dlerror()) != NULL) {
        printf("Error: method %s is not found\n", error_msg);
        dlclose(handle);
        return NULL;
    }

    struct_pkg *module_list = new struct_pkg(new struct_dict());
    for (meth_info *method_obj = init_func(); method_obj->method_name != NULL; method_obj++)
        module_list->put(new obj_str(method_obj->method_name), new struct_func(method_obj->method));
    return module_list;
}
