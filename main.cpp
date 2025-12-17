#include <fstream>

#include "genlib.hpp"

/*
static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}
*/

int main() {
  bytecode bytes;
  bytes.write_header();
  bytes.write_make_constant<std::string>(value_types::string,
                                         std::string("../std/io.so"));
  bytes.write_make_constant<std::string>(value_types::string,
                                         std::string("define_function"));
  bytes.write_make_constant<std::string>(value_types::string,
                                         std::string("print"));
  bytes.write_make_constant<double>(value_types::number, 0);

  bytes.write_var_args();
  bytes.write_load_constant(0);  // io.so
  bytes.write_store_varg();
  bytes.write_load_global(3);  // ffi_open
  bytes.write_call();          // call
  bytes.write_store_local(0);  // local io

  bytes.write_var_args();
  bytes.write_load_local(0);
  bytes.write_load_constant(1);
  bytes.write_get_attr();
  bytes.write_store_varg();

  bytes.write_load_constant(2);
  bytes.write_store_varg();
  bytes.write_load_constant(3);
  bytes.write_store_varg();

  bytes.write_load_local(0);
  bytes.write_load_constant(1);
  bytes.write_get_hash();
  bytes.write_call();
  bytes.write_pop();

  bytes.write_var_args();
  bytes.write_load_local(0);
  bytes.write_load_constant(2);
  bytes.write_get_attr();
  bytes.write_store_varg();

  bytes.write_load_constant(1);
  bytes.write_store_varg();
  bytes.write_load_constant(1);
  bytes.write_store_varg();
  bytes.write_load_constant(1);
  bytes.write_store_varg();
  bytes.write_load_constant(1);
  bytes.write_store_varg();
  bytes.write_load_constant(1);
  bytes.write_store_varg();
  bytes.write_load_constant(1);
  bytes.write_store_varg();
  bytes.write_load_local(0);
  bytes.write_load_constant(2);

  bytes.write_get_hash();
  bytes.write_call();
  bytes.write_pop();

  bytes.write_halt();
  bytes.process_bytecode();

  auto bytecode_array = bytes.get_bytecode();
  std::ofstream output("output", std::ios::out | std::ios::binary);

  for (uint8_t byte : bytecode_array) {
    output.write(reinterpret_cast<const char*>(&byte), sizeof(uint8_t));
  }

  output.close();
  return 0;
};