#include <iostream>

#include "../src/object.hpp"

extern "C" {
object* print(var_args_object* args) {
  std::string buffer;
  int argc = args->value.size();
  int index = 0;
  for (object* obj : args->value) {
    buffer += obj->str() + (index == argc - 1 ? "" : ", ");
    index++;
  }

  std::cout << buffer << std::endl;

  null_object* null = new null_object();
  null->value_type = value_types::null;
  return null;
};
}