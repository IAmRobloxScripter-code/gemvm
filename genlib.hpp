#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#define to_uint8 static_cast<uint8_t>
#define gen_lib_major 0
#define gen_lib_minor 1

enum class block_type : uint8_t {
  global,
  function,
  loop,
  conditional,
};

enum class value_types : uint8_t {
  number,
  string,
  boolean,
  table,
  null,
  function
};

enum class instructions : uint8_t {
  push,
  make_constant,
  load_constant,

  store_local,
  load_local,

  make_frame,
  delete_frame,

  ret,

  jmp,
  jmp_if_true,
  jmp_if_false,

  cmp_eq,
  cmp_neq,
  cmp_gt,
  cmp_lt,
  cmp_gte,
  cmp_lte,
  unary_not,

  and_check,
  or_check,

  add,
  sub,
  mul,
  div,
  pow,
  mod,

  make_function,
  call,
  halt,

  close,
  skip,

  store_upvalue,
  load_upvalue,

  new_table,
  get_array,
  get_hash,
  store_array,
  store_hash,
  load_global,
  define_class,
  get_attr,
  store_method
};

struct label_cache_child {
  std::string label_name;
  uint64_t position_in_bytecode;
};

class bytecode {
 private:
  std::vector<uint8_t> bytecode;
  std::vector<label_cache_child> label_position_cache;
  std::unordered_map<std::string, uint64_t> labels;

 private:
  template <typename T>
  void write(const T& value) {
    const char* bytes = reinterpret_cast<const char*>(&value);
    this->bytecode.insert(this->bytecode.end(), bytes, bytes + sizeof(T));
  };
  template <typename T>
  void write_type(T value, value_types value_type) {
    if constexpr (std::is_same_v<T, double>) {
      this->write<double>(value);
    } else if constexpr (std::is_same_v<T, bool>) {
      this->write<uint8_t>(value ? 1 : 0);
    } else if constexpr (std::is_same_v<T, std::string>) {
      for (char c : value) this->write<char>(c);
      this->write<uint8_t>(0);
    }
  };

 public:
  void write_header() {
    this->write<char>('G');
    this->write<char>('V');
    this->write<char>('M');
    this->write<uint64_t>(gen_lib_major * 10 + gen_lib_minor);
  };
  std::vector<uint8_t> get_bytecode() { return this->bytecode; };

  void process_bytecode() {
    for (label_cache_child cached_position : this->label_position_cache) {
      uint64_t position = this->labels[cached_position.label_name];
      uint8_t x[8];
      std::memcpy(x, &position, 8);

      for (uint8_t i = 0; i < 8; i++) {
        this->bytecode[cached_position.position_in_bytecode - i] = x[7 - i];
      }
    };
  };

  void write_label(const std::string name) {
    labels[name] = this->bytecode.size();
  };

  template <typename T>
  void write_make_constant(value_types value_type, T value) {
    this->write<uint8_t>(to_uint8(instructions::make_constant));
    this->write<uint8_t>(to_uint8(value_type));
    this->write_type<T>(value, value_type);
  };
  void write_load_constant(uint32_t constant) {
    this->write<uint8_t>(to_uint8(instructions::load_constant));
    this->write<uint32_t>(constant);
  };
  void write_store_local(uint8_t id) {
    this->write<uint8_t>(to_uint8(instructions::store_local));
    this->write<uint8_t>(id);
  };
  void write_load_local(uint8_t local) {
    this->write<uint8_t>(to_uint8(instructions::load_local));
    this->write<uint8_t>(local);
  };
  void write_add() { this->write<uint8_t>(to_uint8(instructions::add)); };
  void write_sub() { this->write<uint8_t>(to_uint8(instructions::sub)); };
  void write_mul() { this->write<uint8_t>(to_uint8(instructions::mul)); };
  void write_div() { this->write<uint8_t>(to_uint8(instructions::div)); };
  void write_pow() { this->write<uint8_t>(to_uint8(instructions::pow)); };
  void write_mod() { this->write<uint8_t>(to_uint8(instructions::mod)); };
  void write_make_frame(block_type scope_type, std::vector<uint8_t> up_values) {
    this->write<uint8_t>(to_uint8(instructions::make_frame));
    this->write<uint8_t>(to_uint8(scope_type));
    this->write<uint8_t>(to_uint8(up_values.size() / 2));
    for (uint8_t up_value : up_values) {
      this->write<uint8_t>(up_value);
    }
  };
  void write_delete_frame() {
    this->write<uint8_t>(to_uint8(instructions::delete_frame));
  };
  void write_jmp(const std::string label) {
    this->write<uint8_t>(to_uint8(instructions::jmp));
    this->write<uint64_t>(0);
    this->label_position_cache.push_back(label_cache_child{
        .label_name = label, .position_in_bytecode = bytecode.size() - 1});
  };
  void write_jmp_if_true(const std::string label) {
    this->write<uint8_t>(to_uint8(instructions::jmp_if_true));
    this->write<uint64_t>(0);
    this->label_position_cache.push_back(label_cache_child{
        .label_name = label, .position_in_bytecode = bytecode.size() - 1});
  };
  void write_jmp_if_false(const std::string label) {
    this->write<uint8_t>(to_uint8(instructions::jmp_if_false));
    this->write<uint64_t>(0);
    this->label_position_cache.push_back(label_cache_child{
        .label_name = label, .position_in_bytecode = bytecode.size() - 1});
  };
  void write_cmp_eq() { this->write<uint8_t>(to_uint8(instructions::cmp_eq)); };
  void write_cmp_neq() {
    this->write<uint8_t>(to_uint8(instructions::cmp_neq));
  };
  void write_cmp_gt() { this->write<uint8_t>(to_uint8(instructions::cmp_gt)); };
  void write_cmp_lt() { this->write<uint8_t>(to_uint8(instructions::cmp_lt)); };
  void write_cmp_gte() {
    this->write<uint8_t>(to_uint8(instructions::cmp_gte));
  };
  void write_cmp_lte() {
    this->write<uint8_t>(to_uint8(instructions::cmp_lte));
  };
  void write_unary_not() {
    this->write<uint8_t>(to_uint8(instructions::unary_not));
  };
  void write_and_check() {
    this->write<uint8_t>(to_uint8(instructions::and_check));
  };
  void write_or_check() {
    this->write<uint8_t>(to_uint8(instructions::or_check));
  };
  void write_make_function(const std::string label,
                           std::vector<uint8_t> up_values) {
    this->write<uint8_t>(to_uint8(instructions::make_function));
    this->write<uint64_t>(0);
    this->label_position_cache.push_back(label_cache_child{
        .label_name = label, .position_in_bytecode = bytecode.size() - 1});
    this->write<uint8_t>(to_uint8(up_values.size() / 2));
    for (uint8_t up_value : up_values) {
      this->write<uint8_t>(up_value);
    };
  };
  void write_call() { this->write<uint8_t>(to_uint8(instructions::call)); };
  void write_halt() { this->write<uint8_t>(to_uint8(instructions::halt)); };
  void write_ret() { this->write<uint8_t>(to_uint8(instructions::ret)); };
  void write_close() { this->write<uint8_t>(to_uint8(instructions::close)); };
  void write_skip() { this->write<uint8_t>(to_uint8(instructions::skip)); };
  void write_store_upvalue(uint8_t upvalue) {
    this->write<uint8_t>(to_uint8(instructions::store_upvalue));
    this->write<uint8_t>(upvalue);
  };
  void write_load_upvalue(uint8_t upvalue) {
    this->write<uint8_t>(to_uint8(instructions::load_upvalue));
    this->write<uint8_t>(upvalue);
  };
  void write_new_table() {
    this->write<uint8_t>(to_uint8(instructions::new_table));
  };
  void write_store_array() {
    this->write<uint8_t>(to_uint8(instructions::store_array));
  };
  void write_get_array() {
    this->write<uint8_t>(to_uint8(instructions::get_array));
  };
  void write_store_hash() {
    this->write<uint8_t>(to_uint8(instructions::store_hash));
  };
  void write_get_hash() {
    this->write<uint8_t>(to_uint8(instructions::get_hash));
  };
  void write_load_global(uint8_t global) {
    this->write<uint8_t>(to_uint8(instructions::load_global));
    this->write<uint8_t>(global);
  };
  void write_define_class() {
    this->write<uint8_t>(to_uint8(instructions::define_class));
  };
  void write_get_attr() {
    this->write<uint8_t>(to_uint8(instructions::get_attr));
  };
  void write_store_method() {
    this->write<uint8_t>(to_uint8(instructions::store_method));
  };
};