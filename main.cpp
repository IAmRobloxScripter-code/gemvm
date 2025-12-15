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
                                         std::string("../math.so"));
  bytes.write_make_constant<std::string>(value_types::string,
                                         std::string("define_function"));
  bytes.write_make_constant<std::string>(value_types::string,
                                         std::string("add"));
  bytes.write_make_constant<double>(value_types::number, 3);

  bytes.write_load_constant(0);  // math.so
  bytes.write_load_global(3);    // ffi_open
  bytes.write_call();            // call
  bytes.write_store_local(0);    // local math

  bytes.write_load_local(0);     // math
  bytes.write_load_constant(1);  // define_function
  bytes.write_get_attr();        // self

  bytes.write_load_constant(2);  // add
  bytes.write_load_constant(3);  // 1

  bytes.write_load_local(0);
  bytes.write_load_constant(1);  // define_function
  bytes.write_get_hash();
  bytes.write_call();
  bytes.write_pop();

  bytes.write_load_local(0);     // math
  bytes.write_load_constant(2);  // add
  bytes.write_get_attr();        // self

  bytes.write_load_constant(3);  // 2
  bytes.write_load_constant(3);  // 2

  bytes.write_load_local(0);     // math
  bytes.write_load_constant(2);  // add
  bytes.write_get_hash();
  bytes.write_call();

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