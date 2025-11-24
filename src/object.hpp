#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class value_types : uint8_t {
  number,
  string,
  boolean,
  table,
  null,
  function,
  class_
};

struct function_object;

struct object {
  std::unordered_map<std::string, object*> methods;
  bool marked = false;
  value_types value_type;

  virtual void mark();
  virtual void free();
  virtual void print();

  virtual object* add(object* other);
  virtual object* sub(object* other);
  virtual object* mul(object* other);
  virtual object* div(object* other);
  virtual object* powr(object* other);
  virtual object* modl(object* other);
  virtual object* cmp_eq(object* other);
  virtual object* cmp_neq(object* other);
  virtual object* cmp_gt(object* other);
  virtual object* cmp_lt(object* other);
  virtual object* cmp_gte(object* other);
  virtual object* cmp_lte(object* other);
  virtual object* unary_not(object* other);
  virtual std::string hash();
  virtual object* copy();
  virtual ~object() = default;
};

struct number_object : public object {
  double value;
  number_object(double value);

  void mark() override;
  void free() override;
  void print() override;

  object* add(object* other) override;
  object* sub(object* other) override;
  object* mul(object* other) override;
  object* div(object* other) override;
  object* powr(object* other) override;
  object* modl(object* other) override;
  object* cmp_eq(object* other) override;
  object* cmp_neq(object* other) override;
  object* cmp_gt(object* other) override;
  object* cmp_lt(object* other) override;
  object* cmp_gte(object* other) override;
  object* cmp_lte(object* other) override;
  object* unary_not(object* other) override;
  std::string hash() override;
  object* copy() override;
};

struct string_object : public object {
  std::string value;
  string_object(std::string value);

  void mark() override;
  void free() override;
  void print() override;

  object* add(object* other) override;
  object* sub(object* other) override;
  object* mul(object* other) override;
  object* div(object* other) override;
  object* powr(object* other) override;
  object* modl(object* other) override;
  object* cmp_eq(object* other) override;
  object* cmp_neq(object* other) override;
  object* cmp_gt(object* other) override;
  object* cmp_lt(object* other) override;
  object* cmp_gte(object* other) override;
  object* cmp_lte(object* other) override;
  object* unary_not(object* other) override;
  std::string hash() override;
  object* copy() override;
};

struct boolean_object : public object {
  bool value;
  boolean_object(bool value);

  void mark() override;
  void free() override;
  void print() override;

  object* add(object* other) override;
  object* sub(object* other) override;
  object* mul(object* other) override;
  object* div(object* other) override;
  object* powr(object* other) override;
  object* modl(object* other) override;
  object* cmp_eq(object* other) override;
  object* cmp_neq(object* other) override;
  object* cmp_gt(object* other) override;
  object* cmp_lt(object* other) override;
  object* cmp_gte(object* other) override;
  object* cmp_lte(object* other) override;
  object* unary_not(object* other) override;
  std::string hash() override;

  object* copy() override;
};

struct null_object : public object {
  null_object();

  void mark() override;
  void free() override;
  void print() override;

  object* add(object* other) override;
  object* sub(object* other) override;
  object* mul(object* other) override;
  object* div(object* other) override;
  object* powr(object* other) override;
  object* modl(object* other) override;
  object* cmp_eq(object* other) override;
  object* cmp_neq(object* other) override;
  object* cmp_gt(object* other) override;
  object* cmp_lt(object* other) override;
  object* cmp_gte(object* other) override;
  object* cmp_lte(object* other) override;
  object* unary_not(object* other) override;
  std::string hash() override;

  object* copy() override;
};

enum class function_types {
  gem_function,
  c_function,
  native_function,
};

typedef object* (native_function_ptr)(std::vector<object*> args, void* vm_ptr);

struct native_function {
  uint8_t args;
  native_function_ptr* ptr;
};

struct gem_function {
  std::vector<object**> up_values;
  uint64_t ip;
};

struct function_object : public object {
  function_types function_type;
  union {
    gem_function* gem_function_value;
    native_function* native_function_value;
  };

  function_object();

  void mark() override;
  void free() override;
  void print() override;

  object* add(object* other) override;
  object* sub(object* other) override;
  object* mul(object* other) override;
  object* div(object* other) override;
  object* powr(object* other) override;
  object* modl(object* other) override;
  object* cmp_eq(object* other) override;
  object* cmp_neq(object* other) override;
  object* cmp_gt(object* other) override;
  object* cmp_lt(object* other) override;
  object* cmp_gte(object* other) override;
  object* cmp_lte(object* other) override;
  object* unary_not(object* other) override;
  std::string hash() override;

  object* copy() override;
};

struct table {
  std::vector<object*> array;
  std::unordered_map<std::string, object*> dictionary;

  object* size();
  void push_back(object* value);
  void push_front(object* value);
  object* pop_back();
  object* pop_front();
  object* remove(object* index);
  void insert(object* value, object* index);
};

struct table_object : public object {
  table* value;
  table_object(table* value);

  void mark() override;
  void free() override;
  void print() override;

  object* add(object* other) override;
  object* sub(object* other) override;
  object* mul(object* other) override;
  object* div(object* other) override;
  object* powr(object* other) override;
  object* modl(object* other) override;
  object* cmp_eq(object* other) override;
  object* cmp_neq(object* other) override;
  object* cmp_gt(object* other) override;
  object* cmp_lt(object* other) override;
  object* cmp_gte(object* other) override;
  object* cmp_lte(object* other) override;
  object* unary_not(object* other) override;
  std::string hash() override;

  object* copy() override;
};

// struct ffi_object : public object {
//   std::vector<object*> value;
//   ffi_object();

//   void mark() override;
//   void free() override;
//   void print() override;

//   object* add(object* other) override;
//   object* sub(object* other) override;
//   object* mul(object* other) override;
//   object* div(object* other) override;
//   object* powr(object* other) override;
//   object* modl(object* other) override;
//   object* cmp_eq(object* other) override;
//   object* cmp_neq(object* other) override;
//   object* cmp_gt(object* other) override;
//   object* cmp_lt(object* other) override;
//   object* cmp_gte(object* other) override;
//   object* cmp_lte(object* other) override;
//   object* unary_not(object* other) override;
//   std::string hash() override;

//   object* copy() override;
// };
function_object* make_native(native_function_ptr* ptr, uint8_t argc);
object* __getmember_class_method(std::vector<object*> args, void* vm_ptr);

struct class_object : public object {
  class_object();

  void mark() override;
  void free() override;
  std::string hash() override;
};