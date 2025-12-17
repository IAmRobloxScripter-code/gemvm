#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include "ffi.h"
#include "vm.hpp"

#define __args std::vector<object*> args, void *vm_ptr

object* __getmember_class_ffi_overload(__args) {
  GEM_VIRTUAL_MACHINE* vm = static_cast<GEM_VIRTUAL_MACHINE*>(vm_ptr);
  class_object* self = static_cast<class_object*>(args[0]);
  string_object* fn_name = static_cast<string_object*>(args[1]);

  if (self->methods.find(fn_name->value) != self->methods.end())
    return self->methods[fn_name->value];
  return vm->globals[0];
};

object* define_function(__args) {
  GEM_VIRTUAL_MACHINE* vm = static_cast<GEM_VIRTUAL_MACHINE*>(vm_ptr);
  class_object* self = static_cast<class_object*>(args[0]);
  string_object* defined_function = static_cast<string_object*>(args[1]);
  number_object* argc = static_cast<number_object*>(args[2]);

  void* ptr =
      dlsym(static_cast<pointer_object*>(self->methods["__ffi_handle_pointer"])
                ->value,
            defined_function->value.c_str());
  if (!ptr) {
    std::cerr << dlerror() << "\n";
    vm->exit_handler(1);
    return nullptr;
  }

  function_object* ffi_function_object = new function_object();
  ffi_function_object->function_type = function_types::ffi_function;
  foreign_function* ffi_function = new foreign_function();
  ffi_function->foreign = reinterpret_cast<ffi_function_ptr>(ptr);
  ffi_function->args = argc->value;
  ffi_function_object->foreign_function_value = ffi_function;
  self->methods[defined_function->value] = ffi_function_object;
  return vm->globals[0];
};

object* ffi_open(__args) {
  GEM_VIRTUAL_MACHINE* vm = static_cast<GEM_VIRTUAL_MACHINE*>(vm_ptr);
  if (args.size() < 1) {
    std::cerr << "Too few arguments.\n";
    vm->exit_handler(1);
    return nullptr;
  }
  std::string cpp_module_path = static_cast<string_object*>(args[0])->value;
  class_object* ffi_class = new class_object();
  ffi_class->value_type = value_types::class_;
  table* ffi_functions_args_table = new table;
  table_object* ffi_functions_args_object =
      new table_object(ffi_functions_args_table);
  ffi_functions_args_object->value_type = value_types::table;
  ffi_class->methods["define_function"] = make_native(&define_function, 3);

#ifdef _WIN32
  HINSTANCE handle = LoadLibrary(cpp_module_path);
  if (!handle) {
    std::cerr << "Could not load library." << std::endl;
    vm->exit_handler(1);
    return nullptr;
  }

  ffi_class->methods["__getmember"] =
      make_native(&__getmember_class_ffi_overload, 2);
  ffi_class->methods["__ffi_handle_pointer"] = new pointer_object(handle);
#else
  void* handle = dlopen(cpp_module_path.c_str(), RTLD_LAZY);

  if (!handle) {
    std::cerr << "Could not load library." << std::endl;
    vm->exit_handler(1);
    return nullptr;
  }

  ffi_class->methods["__getmember"] =
      make_native(&__getmember_class_ffi_overload, 2);
  ffi_class->methods["__ffi_handle_pointer"] = new pointer_object(handle);
#endif

  return ffi_class;
}

GEM_VIRTUAL_MACHINE::GEM_VIRTUAL_MACHINE() {
  null_object* null_value = new null_object();
  boolean_object* false_value = new boolean_object(false);
  boolean_object* true_value = new boolean_object(true);
  null_value->value_type = value_types::null;
  false_value->value_type = value_types::boolean;
  true_value->value_type = value_types::boolean;

  this->globals.push_back(null_value);
  this->globals.push_back(false_value);
  this->globals.push_back(true_value);
  this->globals.push_back(make_native(&ffi_open, 1));
}