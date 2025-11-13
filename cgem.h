#ifndef __CGEM_H
#define __CGEM_H

#define c_new(T, V) (T*)malloc(sizeof(T) * V)
#define c_resize(PTR, T, V) (T*)realloc(PTR, sizeof(T) * V)
#define c_size_t unsigned char
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  c_number_type,
  c_string_type,
  c_boolean_type,
  c_function_type,
  c_table_type,
  c_null_type
} c_object_types;

typedef enum {
  c_gem_function_type,
  c_c_function_type,
  c_native_function_type,
} c_function_types;

struct c_object;
struct c_table;
struct c_function;
typedef void (*function_ptr)(struct c_object* args, c_size_t argc);

typedef struct c_table_entry {
  char* key;
  struct c_object* value;
} c_table_entry;

typedef struct c_object {
  c_object_types value_type;
  union {
    double number;
    char* string;
    bool boolean;
    c_function* function;
    struct c_table* table;
  } value;
} c_object;

typedef struct c_table {
  c_object** array;
  c_table_entry* dictionary;

  uint64_t array_capacity;
  uint64_t array_size;

  uint64_t dictionary_capacity;
  uint64_t dictionary_size;
} c_table;

typedef struct c_function {
  c_function_types function_type;
  union cgem {
    uint64_t gem_function;
    void* native_function;
    function_ptr c_function;
  } function;
} c_function;

c_object* create_c_object(c_object_types value_type) {
  c_object* object = c_new(c_object, 1);
  object->value_type = value_type;

  if (value_type == c_string_type) {
    object->value.string = c_new(char, 1);
    object->value.string[0] = '\0';
  }

  return object;
}

c_object* c_string_concat(c_object* x, c_object* y) {
  c_size_t len_x = strlen(x->value.string);
  c_size_t len_y = strlen(y->value.string);
  c_size_t xy_len = len_x + len_y + 1;

  c_object* result = c_new(c_object, 1);
  result->value_type = c_string_type;
  result->value.string = c_new(char, xy_len);

  strcpy(result->value.string, y->value.string);
  strcat(result->value.string, x->value.string);

  return result;
}

c_table* create_c_table() {
  c_table* table = c_new(c_table, 1);
  table->array_capacity = 16;
  table->dictionary_capacity = 16;
  table->array = c_new(c_object*, table->array_capacity);
  table->dictionary = c_new(c_table_entry, table->dictionary_capacity);

  return table;
};

void c_table_push_back(c_object* table_ptr, c_object* value) {
  c_table* table = table_ptr->value.table;

  if (table->array_size > table->array_capacity) {
    table->array_capacity *= 2;
    table->array = c_resize(table->array, c_object*, table->array_capacity);
  }

  table->array[table->array_size] = value;
  table->array_size++;
}

void c_table_append(c_object* table_ptr, const char* key, c_object* value) {
  c_table* table = table_ptr->value.table;

  if (table->dictionary_size > table->dictionary_capacity) {
    table->dictionary_capacity *= 2;
    table->dictionary =
        c_resize(table->dictionary, c_table_entry, table->dictionary_capacity);
  }

  for (c_size_t index = 0; index < table->dictionary_size; ++index) {
    if (!strcmp(table->dictionary[index].key, key)) {
      table->dictionary[index].value = value;
      return;
    }
  }

  c_table_entry entry = {};
  entry.key = c_new(char, strlen(key) + 1);
  strcpy(entry.key, key);
  entry.value = value;

  table->dictionary[table->dictionary_size] = entry;
  table->dictionary_size++;
}

c_object* c_table_array_at(c_object* table_ptr, c_size_t index) {
  return table_ptr->value.table->array[index];
}

c_object* c_table_dictionary_at(c_object* table_ptr, char* key) {
  c_table* table = table_ptr->value.table;

  for (c_size_t index = 0; index < table->dictionary_size; ++index) {
    if (!strcmp(table->dictionary[index].key, key)) {
      return table->dictionary[index].value;
    }
  }

  return 0;
}

void c_object_free(c_object* object) {
  free(object->value.string);
  free(object);
}

void c_table_free(c_table* table) {
  free(table->dictionary);
  free(table->array);
  free(table);
}

#endif