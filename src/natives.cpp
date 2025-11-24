#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <vm.hpp>
#include "ffi.h"

#define __args std::vector<object*> args, GEM_VIRTUAL_MACHINE *vm

object* ffi_open(__args) {
  if (args.size() < 1) {
    std::cerr << "Too few arguments.\n";
    vm->exit_handler(1);
    return nullptr;
  }

  std::string cpp_module_path = static_cast<string_object*>(args[0])->value;

  #ifdef _WIN32
    HINSTANCE handle = LoadLibrary(cpp_module_path);
    if (!handle) {
        std::cerr << "Could not load library." << std::endl;
        vm->exit_handler(1);
    }
  #else 

  #endif

}

GEM_VIRTUAL_MACHINE::GEM_VIRTUAL_MACHINE() {}