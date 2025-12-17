#include "object.hpp"

#include <cmath>
#include <iostream>

function_object* make_native(native_function_ptr* ptr, uint8_t argc) {
  function_object* obj = new function_object();
  obj->function_type = function_types::native_function;
  native_function* native_func_obj = new native_function();
  native_func_obj->ptr = *ptr;
  native_func_obj->args = argc;
  obj->native_function_value = native_func_obj;

  return obj;
}

number_object::number_object(double value) { this->value = value; }
string_object::string_object(std::string value) { this->value = value; }
boolean_object::boolean_object(bool value) { this->value = value; }
null_object::null_object() = default;
function_object::function_object() = default;
table_object::table_object(table* value) { this->value = value; }
var_args_object::var_args_object() = default;
// ffi_object::ffi_object() = default;

object* object::add(object* other) { return nullptr; };
object* object::sub(object* other) { return nullptr; };
object* object::mul(object* other) { return nullptr; };
object* object::div(object* other) { return nullptr; };
object* object::powr(object* other) { return nullptr; };
object* object::modl(object* other) { return nullptr; };
object* object::cmp_eq(object* other) { return nullptr; };
object* object::cmp_neq(object* other) { return nullptr; };
object* object::cmp_gt(object* other) { return nullptr; };
object* object::cmp_lt(object* other) { return nullptr; };
object* object::cmp_gte(object* other) { return nullptr; };
object* object::cmp_lte(object* other) { return nullptr; };
object* object::unary_not(object* other) { return nullptr; };

object* number_object::add(object* other) {
  double value = this->value + static_cast<number_object*>(other)->value;
  object* result = new number_object(value);
  result->value_type = value_types::number;
  return result;
}

object* number_object::sub(object* other) {
  double value = this->value - static_cast<number_object*>(other)->value;
  number_object* result = new number_object(value);
  result->value_type = value_types::number;
  return result;
}

object* number_object::mul(object* other) {
  double value = this->value * static_cast<number_object*>(other)->value;
  number_object* result = new number_object(value);
  result->value_type = value_types::number;
  return result;
}

object* number_object::div(object* other) {
  double value = this->value / static_cast<number_object*>(other)->value;
  number_object* result = new number_object(value);
  result->value_type = value_types::number;
  return result;
}

object* number_object::powr(object* other) {
  double value = pow(this->value, static_cast<number_object*>(other)->value);
  number_object* result = new number_object(value);
  result->value_type = value_types::number;
  return result;
}

object* number_object::modl(object* other) {
  double value = fmod(this->value, static_cast<number_object*>(other)->value);
  number_object* result = new number_object(value);
  result->value_type = value_types::number;
  return result;
}

object* number_object::cmp_eq(object* other) {
  bool value = this->value == static_cast<number_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* number_object::cmp_neq(object* other) {
  bool value = this->value != static_cast<number_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* number_object::cmp_gt(object* other) {
  bool value = this->value > static_cast<number_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* number_object::cmp_lt(object* other) {
  bool value = this->value < static_cast<number_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* number_object::cmp_gte(object* other) {
  bool value = this->value >= static_cast<number_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* number_object::cmp_lte(object* other) {
  bool value = this->value <= static_cast<number_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* number_object::unary_not(object* other) {
  bool value = !this->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* string_object::add(object* other) {
  std::string value = this->value + static_cast<string_object*>(other)->value;
  string_object* result = new string_object(value);
  result->value_type = value_types::string;
  return result;
}

object* string_object::sub(object* other) { return nullptr; }
object* string_object::mul(object* other) { return nullptr; }
object* string_object::div(object* other) { return nullptr; }
object* string_object::powr(object* other) { return nullptr; }
object* string_object::modl(object* other) { return nullptr; }
object* string_object::cmp_eq(object* other) {
  bool value = this->value == static_cast<string_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* string_object::cmp_neq(object* other) {
  bool value = this->value != static_cast<string_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* string_object::cmp_gt(object* other) { return nullptr; }
object* string_object::cmp_lt(object* other) { return nullptr; }
object* string_object::cmp_gte(object* other) { return nullptr; }
object* string_object::cmp_lte(object* other) { return nullptr; }

object* string_object::unary_not(object* other) {
  bool value = !(this->value.size() == 0 ? false : true);
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* boolean_object::add(object* other) { return nullptr; };
object* boolean_object::sub(object* other) { return nullptr; };
object* boolean_object::mul(object* other) { return nullptr; };
object* boolean_object::div(object* other) { return nullptr; };
object* boolean_object::powr(object* other) { return nullptr; };
object* boolean_object::modl(object* other) { return nullptr; };
object* boolean_object::cmp_eq(object* other) {
  bool value = this->value == static_cast<boolean_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
};

object* boolean_object::cmp_neq(object* other) {
  bool value = this->value != static_cast<boolean_object*>(other)->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
};

object* boolean_object::cmp_gt(object* other) { return nullptr; };
object* boolean_object::cmp_lt(object* other) { return nullptr; };
object* boolean_object::cmp_gte(object* other) { return nullptr; };
object* boolean_object::cmp_lte(object* other) { return nullptr; };
object* boolean_object::unary_not(object* other) {
  bool value = !this->value;
  boolean_object* result = new boolean_object(value);
  result->value_type = value_types::boolean;
  return result;
}

object* null_object::add(object* other) { return nullptr; };
object* null_object::sub(object* other) { return nullptr; };
object* null_object::mul(object* other) { return nullptr; };
object* null_object::div(object* other) { return nullptr; };
object* null_object::powr(object* other) { return nullptr; };
object* null_object::modl(object* other) { return nullptr; };
object* null_object::cmp_eq(object* other) { return nullptr; };
object* null_object::cmp_neq(object* other) { return nullptr; };
object* null_object::cmp_gt(object* other) { return nullptr; };
object* null_object::cmp_lt(object* other) { return nullptr; };
object* null_object::cmp_gte(object* other) { return nullptr; };
object* null_object::cmp_lte(object* other) { return nullptr; };
object* null_object::unary_not(object* other) { return nullptr; };

object* function_object::add(object* other) { return nullptr; };
object* function_object::sub(object* other) { return nullptr; };
object* function_object::mul(object* other) { return nullptr; };
object* function_object::div(object* other) { return nullptr; };
object* function_object::powr(object* other) { return nullptr; };
object* function_object::modl(object* other) { return nullptr; };
object* function_object::cmp_eq(object* other) { return nullptr; };
object* function_object::cmp_neq(object* other) { return nullptr; };
object* function_object::cmp_gt(object* other) { return nullptr; };
object* function_object::cmp_lt(object* other) { return nullptr; };
object* function_object::cmp_gte(object* other) { return nullptr; };
object* function_object::cmp_lte(object* other) { return nullptr; };
object* function_object::unary_not(object* other) { return nullptr; };

object* table_object::add(object* other) { return nullptr; };
object* table_object::sub(object* other) { return nullptr; };
object* table_object::mul(object* other) { return nullptr; };
object* table_object::div(object* other) { return nullptr; };
object* table_object::powr(object* other) { return nullptr; };
object* table_object::modl(object* other) { return nullptr; };
object* table_object::cmp_eq(object* other) { return nullptr; };
object* table_object::cmp_neq(object* other) { return nullptr; };
object* table_object::cmp_gt(object* other) { return nullptr; };
object* table_object::cmp_lt(object* other) { return nullptr; };
object* table_object::cmp_gte(object* other) { return nullptr; };
object* table_object::cmp_lte(object* other) { return nullptr; };
object* table_object::unary_not(object* other) { return nullptr; };

// object* ffi_object::add(object* other) { return nullptr; };
// object* ffi_object::sub(object* other) { return nullptr; };
// object* ffi_object::mul(object* other) { return nullptr; };
// object* ffi_object::div(object* other) { return nullptr; };
// object* ffi_object::powr(object* other) { return nullptr; };
// object* ffi_object::modl(object* other) { return nullptr; };
// object* ffi_object::cmp_eq(object* other) { return nullptr; };
// object* ffi_object::cmp_neq(object* other) { return nullptr; };
// object* ffi_object::cmp_gt(object* other) { return nullptr; };
// object* ffi_object::cmp_lt(object* other) { return nullptr; };
// object* ffi_object::cmp_gte(object* other) { return nullptr; };
// object* ffi_object::cmp_lte(object* other) { return nullptr; };
// object* ffi_object::unary_not(object* other) { return nullptr; };

void object::free() {};
void number_object::free() {};
void string_object::free() { this->value.~basic_string(); };
void boolean_object::free() {};
void null_object::free() {};
void function_object::free() {};
void table_object::free() { delete this->value; };
// void ffi_object::free() {};
void class_object::free() {};

void object::mark() { this->marked = true; };
void number_object::mark() { this->marked = true; };
void string_object::mark() { this->marked = true; };
void boolean_object::mark() { this->marked = true; };
void null_object::mark() { this->marked = true; };
void function_object::mark() {
  this->marked = true;

  if (this->function_type == function_types::gem_function)
    for (object** up_value : this->gem_function_value->up_values)
      (*up_value)->mark();
};
void table_object::mark() {
  for (object* arr_object : this->value->array) arr_object->mark();
  for (auto dict_object : this->value->dictionary) dict_object.second->mark();
  this->marked = true;
};
// void ffi_object::mark() {
//   this->marked = true;
//   for (object* ffi_object : this->value) ffi_object->mark();
// };
void class_object::mark() {
  this->marked = true;
  for (auto fn_obj : this->methods) {
    fn_obj.second->mark();
  }
}
std::string object::str() { return "object"; };
std::string number_object::str() { return std::to_string(this->value); };
std::string string_object::str() { return this->value; };
std::string boolean_object::str() {
  return this->value == false ? "false" : "true";
};
std::string null_object::str() { return "null"; };
std::string function_object::str() {
  return "<function: " + std::to_string(reinterpret_cast<uintptr_t>(this)) +
         ">";
};
std::string table_object::str() {
  return "<table: " + std::to_string(reinterpret_cast<uintptr_t>(this)) + ">";
};
std::string class_object::str() {
  return "<class: " + std::to_string(reinterpret_cast<uintptr_t>(this)) + ">";
}
// void ffi_object::str() {
//   return "<ffi_object: " << this << ">" << std::endl;
// };

object* object::copy() { return nullptr; };
object* number_object::copy() {
  number_object* obj = new number_object(this->value);
  obj->value_type = this->value_type;
  return obj;
}
object* string_object::copy() {
  string_object* obj = new string_object(this->value);
  obj->value_type = this->value_type;
  return obj;
}
object* boolean_object::copy() {
  boolean_object* obj = new boolean_object(this->value);
  obj->value_type = this->value_type;
  return obj;
}
object* null_object::copy() {
  null_object* obj = new null_object();
  obj->value_type = this->value_type;
  return obj;
}
object* function_object::copy() { return nullptr; }
// object* ffi_object::copy() { return nullptr; }

object* table_object::copy() {
  table* new_value = new table(*this->value);
  table_object* new_table = new table_object(new_value);
  new_table->value_type = value_types::table;
  return new_table;
};

std::string object::hash() {
  return "o_" + std::to_string(reinterpret_cast<uintptr_t>(this));
}
std::string number_object::hash() { return "n_" + std::to_string(this->value); }
std::string string_object::hash() { return "s_" + this->value; }
std::string boolean_object::hash() {
  return "b_" + std::to_string(this->value);
}
std::string null_object::hash() { return "n_null"; }
std::string function_object::hash() {
  return "f_" + std::to_string(reinterpret_cast<uintptr_t>(this));
}
std::string table_object::hash() {
  return "t_" + std::to_string(reinterpret_cast<uintptr_t>(this));
}
// std::string ffi_object::hash() {
//   std::stringstream hashed;
//   hashed << "i_" << this;
//   return hashed.str();
// }
class_object::class_object() {
  this->methods["__getmember"] = make_native(&__getmember_class_method, 2);
};

std::string class_object::hash() {
  return "c_" + std::to_string(reinterpret_cast<uintptr_t>(this));
}

object* __getmember_class_method(std::vector<object*> args, void* vm_ptr) {
  return args[0]->methods[static_cast<string_object*>(args[1])->value];
}

pointer_object::pointer_object(void* value) { this->value = value; }
void pointer_object::mark() { this->marked = true; }
void pointer_object::free() {};

std::string pointer_object::hash() {
  return "h_" + std::to_string(reinterpret_cast<uintptr_t>(this));
}

std::string var_args_object::hash() {
  std::string hashed = "v_";

  for (object* obj : this->value) {
    hashed += obj->hash();
  }
  return hashed;
}

void var_args_object::mark() {
  this->marked = true;
  for (object* obj : this->value) obj->mark();
}

void var_args_object::free() {};

std::string var_args_object::str() {
  return "<var_args: " + std::to_string(reinterpret_cast<uintptr_t>(this)) +
         ">";
}