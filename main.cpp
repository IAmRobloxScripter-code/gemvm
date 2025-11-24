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
  bytes.write_make_constant<std::string>(value_types::string, std::string("foo"));
  bytes.write_make_constant<double>(value_types::number, 100);

  bytes.write_jmp("foo_end");
  bytes.write_label("foo");
  bytes.write_store_local(0);
  bytes.write_add();
  bytes.write_ret();
  bytes.write_label("foo_end");

  bytes.write_define_class();
  bytes.write_load_constant(0);
  bytes.write_make_function("foo", std::vector<uint8_t>());
  bytes.write_store_method();
  bytes.write_store_local(0);

  bytes.write_load_constant(1);
  bytes.write_load_constant(1);
  bytes.write_load_local(0);
  bytes.write_load_constant(0);
  bytes.write_get_attr();
  bytes.write_load_local(0);
  bytes.write_load_constant(0);
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