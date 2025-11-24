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
    case c_function_type: {
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
          function->function.native_function =
              cpp_function->native_function_value;
          function->function_type = c_native_function_type;
          break;
        default:
          function->function_type = c_gem_function_type;
          break;
      }

      c_obj_ptr->value.function = function;
      break;
    }
    default:
      break;
  }
  return c_obj_ptr;
}

object* c_cpp_convert(c_object* c_obj_ptr) {
  switch (c_obj_ptr->value_type) {
    case c_number_type: {
      number_object* number_obj_ptr =
          new number_object(c_obj_ptr->value.number);
      number_obj_ptr->value_type = value_types::number;

      free(c_obj_ptr);
      return number_obj_ptr;
    }
    case c_string_type: {
      string_object* string_obj_ptr =
          new string_object(std::string(c_obj_ptr->value.string));
      string_obj_ptr->value_type = value_types::string;

      free(c_obj_ptr);
      return string_obj_ptr;
    }
    case c_boolean_type: {
      boolean_object* boolean_obj_ptr =
          new boolean_object(c_obj_ptr->value.boolean);
      boolean_obj_ptr->value_type = value_types::boolean;

      free(c_obj_ptr);
      return boolean_obj_ptr;
    }
    case c_table_type: {
      table* table_ptr = new table;

      for (uint64_t index = 0; index < c_obj_ptr->value.table->array_size;
           ++index) {
        table_ptr->array.push_back(
            c_cpp_convert(c_obj_ptr->value.table->array[index]));
      }

      for (uint64_t index = 0; index < c_obj_ptr->value.table->dictionary_size;
           ++index) {
        table_ptr->dictionary[std::string(
            c_obj_ptr->value.table->dictionary[index].key)] =
            c_cpp_convert(c_obj_ptr->value.table->dictionary[index].value);
      }

      table_object* table_obj_ptr = new table_object(table_ptr);
      table_obj_ptr->value_type = value_types::table;

      free(c_obj_ptr->value.table);
      free(c_obj_ptr);
      return table_obj_ptr;
    }
    default: {
      null_object* null_obj_ptr = new null_object();
      null_obj_ptr->value_type = value_types::null;

      free(c_obj_ptr);
      return null_obj_ptr;
    }
  }
}