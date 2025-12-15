#pragma once
#include <cstring>
#include <iostream>

#include "instructions.hpp"
#include "stack_frame.hpp"

#define call_stack_limit 64
#define gvm_major 0
#define gvm_minor 1
#define header_size sizeof(char) * 3 + sizeof(uint64_t)
#define gc_trigger_allocation_count 250

static std::vector<object*> object_heap;
static std::vector<stack_frame*> stack_frame_heap;

class GEM_VIRTUAL_MACHINE {
 public:
  std::vector<object*> object_stack;
  std::vector<stack_frame*> stack_frame_stack;
  std::vector<uint64_t> return_ip_stack;
  std::vector<object*> constants;
  GEM_VIRTUAL_MACHINE();
  /*globals ID
    null - 0
    false - 1
    true - 2*/
  std::vector<object*> globals;
  uint64_t ip = 0;
  bool halted = false;
  uint64_t allocations = 0;

  std::vector<uint8_t> bytes;

 public:
  void is_valid_header();
  void execute();
  void exit_handler(uint8_t exit_code);
  void garbage_collect();
  void tick_gc();
  object* pop() {
    if (this->object_stack.size() <= 0) {
      return this->globals[0];
    }
    object* value = this->object_stack.back();
    this->object_stack.pop_back();
    return value;
  };
  stack_frame* get_current_frame() {
    if (this->stack_frame_stack.size() == 0) {
      std::cerr << "Stack underflow!\n";
      this->exit_handler(1);
      return nullptr;
    }
    return this->stack_frame_stack.back();
  };
  void call_function();
  template <typename T>
  T read() {
    T value;
    std::memcpy(&value, &this->bytes[this->ip], sizeof(T));
    this->ip += sizeof(T);
    return value;
  }
};
