#pragma once
#include "../cgem.h"
#include "object.hpp"

c_object_types cpp_c_type_conversion(value_types value_type) {
  switch (value_type) {
    case value_types::number:
      return c_number_type;
    case value_types::string:
      return c_string_type;
    case value_types::boolean:
      return c_boolean_type;
    case value_types::table:
      return c_table_type;
    case value_types::function:
      return c_function_type;
    default:
      return c_null_type;
  }
}

c_object* cpp_c_convert(object* obj_ptr) {
  c_object* c_obj_ptr = c_new(c_object, 1);
  c_obj_ptr->value_type = cpp_c_type_conversion(obj_ptr->value_type);

  switch (c_obj_ptr->value_type) {
    case c_number_type:
      c_obj_ptr->value.number = static_cast<number_object*>(obj_ptr)->value;
      break;
    case c_string_type: {
      char* c_str = c_new(
          char,
          strlen(static_cast<string_object*>(obj_ptr)->value.c_str()) + 1);
      strcpy(c_str, static_cast<string_object*>(obj_ptr)->value.c_str());
      c_obj_ptr->value.string = c_str;
      break;
    }
    case c_boolean_type:
      c_obj_ptr->value.boolean = static_cast<boolean_object*>(obj_ptr)->value;
      break;
    case c_table_type: {
      c_table* c_table_v = create_c_table();
      c_obj_ptr->value.table = c_table_v;
      table* cpp_table_v = static_cast<table_object*>(obj_ptr)->value;

      for (object* array_child : cpp_table_v->array) {
        c_table_push_back(c_obj_ptr, cpp_c_convert(array_child));
      }

      for (auto dictionary_entry : cpp_table_v->dictionary) {
        c_table_append(c_obj_ptr, dictionary_entry.first.c_str(),
                       cpp_c_convert(dictionary_entry.second));
      }
      break;
    }
    case c_function_type:
      c_function* function = c_new(c_function, 1);
      function_object* cpp_function = static_cast<function_object*>(obj_ptr);

      switch (cpp_function->function_type) {
        case function_types::gem_function:
          function->function.gem_function =
              cpp_function->gem_function_value->ip;
          function->function_type = c_gem_function_type;
          break;
        case function_types::c_function:
          function->function_type = c_c_function_type;
          break;
        case function_types::native_function:
          function->function_type = c_native_function_type;
          break;
        default:
          function->function_type = c_gem_function_type;
          break;
      }

      break;
    default:
      break;
  }
}