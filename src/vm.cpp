#include "vm.hpp"

#include <ffi.h>

#include <algorithm>
#include <fstream>
#include <iostream>

void GEM_VIRTUAL_MACHINE::exit_handler(uint8_t exit_code) {
  for (object* obj : this->object_stack) {
    obj->print();
  }

  exit(exit_code);
}

void GEM_VIRTUAL_MACHINE::call_function() {
  function_object* function = static_cast<function_object*>(this->pop());
  switch (function->function_type) {
    case function_types::gem_function: {
      stack_frame* frame = new stack_frame;
      frame->scope_type = block_type::function;
      frame->up_values = function->gem_function_value->up_values;
      this->stack_frame_stack.push_back(frame);
      stack_frame_heap.push_back(frame);
      this->tick_gc();

      this->return_ip_stack.push_back(this->ip);
      this->ip = function->gem_function_value->ip;
      break;
    }
    case function_types::native_function: {
      std::vector<object*> args;

      for (uint8_t index = 0; index < function->native_function_value->args;
           ++index) {
        args.push_back(this->pop());
      }

      std::reverse(args.begin(), args.end());
      object* result = (*function->native_function_value->ptr)(args, this);
      this->object_stack.push_back(result);
      object_heap.push_back(result);
      this->tick_gc();
      break;
    }
    case function_types::ffi_function: {
      std::vector<object*> args;

      for (uint8_t index = 0; index < function->foreign_function_value->args;
           ++index) {
        args.push_back(this->pop());
      }
      args.pop_back();
      std::reverse(args.begin(), args.end());
      ffi_cif cif;
      std::vector<ffi_type*> arg_types;
      std::vector<void*> arg_pointers;
      for (object* obj : args) {
        arg_pointers.push_back(&obj);
        arg_types.push_back(&ffi_type_pointer);
      }

      if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_pointers.size(),
                       &ffi_type_pointer, arg_types.data()) != FFI_OK)
        this->exit_handler(1);

      object* result = nullptr;

      ffi_call(&cif, (void (*)())function->foreign_function_value->foreign,
               &result, arg_pointers.data());

      this->object_stack.push_back(result);
      object_heap.push_back(result);
      this->tick_gc();
      break;
    }
    default:
      break;
  }
}

void GEM_VIRTUAL_MACHINE::is_valid_header() {
  if (this->bytes.size() < header_size) {
    std::cerr << "Invalid byte size!\n Required: " << header_size
              << "\n Got: " << this->bytes.size() << "\n";
    this->exit_handler(1);
    return;
  }
  const uint8_t unique_id_size = header_size - sizeof(uint64_t);
  std::string unique_id;

  for (uint8_t index = 0; index < unique_id_size; ++index) {
    unique_id += this->read<char>();
  }

  uint64_t version = this->read<uint64_t>();

  if (unique_id != "GVM") {
    std::cerr << "Invalid bytecode!\n";
    this->exit_handler(1);
    return;
  }
  if (version > gvm_major * 10 + gvm_minor) {
    std::cerr << "Unsupported version!\n";
    this->exit_handler(1);
    return;
  }
  return;
}

void GEM_VIRTUAL_MACHINE::execute() {
  this->is_valid_header();
  uint64_t program_size = this->bytes.size();

  while (!this->halted && this->ip < program_size) {
    instructions instruction = static_cast<instructions>(this->read<uint8_t>());
    // std::cout << (int)instruction << std::endl;
    switch (instruction) {
      case instructions::make_constant: {
        value_types value_type =
            static_cast<value_types>(this->read<uint8_t>());

        switch (value_type) {
          case value_types::number: {
            number_object* obj = new number_object(this->read<double>());
            obj->value_type = value_type;
            this->constants.push_back(obj);
            break;
          }
          case value_types::string: {
            std::string value;

            while (this->ip < program_size && bytes[this->ip] != 0) {
              value += this->read<char>();
            }
            this->read<uint8_t>();

            string_object* obj = new string_object(value);
            obj->value_type = value_type;
            this->constants.push_back(obj);
            break;
          }
          case value_types::boolean: {
            boolean_object* obj = new boolean_object(this->read<uint8_t>());
            obj->value_type = value_type;
            this->constants.push_back(obj);
            break;
          }
          default:
            std::cerr << "Invalid value type!\n";
            this->exit_handler(1);
            break;
        }
        break;
      };
      case instructions::load_constant: {
        uint32_t constant_id = this->read<u_int32_t>();
        if (this->constants.size() < constant_id) {
          std::cerr << "Constant does not exist!\n";
          this->exit_handler(1);
          break;
        }
        object* constant = this->constants[constant_id];

        this->object_stack.push_back(constant);
        object_heap.push_back(constant);
        this->tick_gc();
        break;
      }
      case instructions::make_frame: {
        block_type scope_type = static_cast<block_type>(this->read<uint8_t>());
        uint8_t up_value_count = this->read<uint8_t>();
        stack_frame* parent = this->get_current_frame();
        stack_frame* frame = new stack_frame;
        frame->scope_type = scope_type;

        for (uint8_t up_value_index = 0; up_value_index < up_value_count;
             ++up_value_index) {
          bool is_local = this->read<uint8_t>();
          if (is_local) {
            frame->up_values.push_back(
                &parent->local_values[this->read<uint8_t>()]);
          } else {
            frame->up_values.push_back(
                parent->up_values[this->read<uint8_t>()]);
          }
        }

        this->stack_frame_stack.push_back(frame);
        stack_frame_heap.push_back(frame);
        this->tick_gc();

        if (this->stack_frame_stack.size() > call_stack_limit) {
          std::cerr << "Call stack limit!\n";
          this->exit_handler(1);
          break;
        }
        break;
      }
      case instructions::delete_frame:
        this->stack_frame_stack.pop_back();
        break;
      case instructions::store_local: {
        object* value = this->pop();
        uint8_t id = this->read<uint8_t>();
        stack_frame* frame = this->get_current_frame();
        if (frame->local_values.size() < id + 1) {
          frame->local_values.resize(id + 1);
          frame->local_values[id] = value;
        } else {
          frame->local_values[id] = value;
        }
        break;
      }
      case instructions::load_local:
        this->object_stack.push_back(
            this->get_current_frame()->local_values[this->read<uint8_t>()]);
        break;
      case instructions::store_upvalue: {
        object* value = this->pop();
        uint8_t id = this->read<uint8_t>();
        stack_frame* frame = this->get_current_frame();
        *frame->up_values[id] = value;
        break;
      }
      case instructions::load_upvalue:
        this->object_stack.push_back(
            *(this->get_current_frame()->up_values[this->read<uint8_t>()]));
        break;
      case instructions::add: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = x->add(y);
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__add"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();
        break;
      }
      case instructions::sub: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = x->sub(y);
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__sub"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::mul: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = x->mul(y);
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__mul"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::div: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = x->div(y);
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__div"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::pow: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = x->powr(y);
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__pow"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::mod: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = x->modl(y);
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__mod"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }

      case instructions::jmp: {
        this->ip = this->read<uint64_t>();
        break;
      };
      case instructions::jmp_if_true: {
        boolean_object* truthy = static_cast<boolean_object*>(this->pop());
        uint64_t jmp_ip = this->read<uint64_t>();

        if (truthy->value == true) {
          this->ip = jmp_ip;
        }
        break;
      }
      case instructions::jmp_if_false: {
        boolean_object* truthy = static_cast<boolean_object*>(this->pop());
        uint64_t jmp_ip = this->read<uint64_t>();
        if (truthy->value == false) {
          this->ip = jmp_ip;
        }
        break;
      }
      case instructions::cmp_eq: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = static_cast<boolean_object*>(x->cmp_eq(y));
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__eq"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::cmp_neq: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = static_cast<boolean_object*>(x->cmp_neq(y));
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__neq"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::cmp_gt: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = static_cast<boolean_object*>(x->cmp_gt(y));
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__gt"]);
          this->call_function();
          break;
        }
        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::cmp_lt: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = static_cast<boolean_object*>(x->cmp_lt(y));
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);

          this->object_stack.push_back(x->methods["__lt"]);
          this->call_function();
          break;
        }

        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::cmp_gte: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = static_cast<boolean_object*>(x->cmp_gte(y));
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(y);
          this->object_stack.push_back(x->methods["__gte"]);
          this->call_function();
          break;
        }

        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::cmp_lte: {
        object* y = this->pop();
        object* x = this->pop();

        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = static_cast<boolean_object*>(x->cmp_lte(y));
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(x->methods["__lte"]);
          this->call_function();
          break;
        }

        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::unary_not: {
        object* x = this->pop();
        object* result = nullptr;

        if (x->value_type != value_types::class_)
          result = static_cast<boolean_object*>(x->unary_not(nullptr));
        else {
          this->object_stack.push_back(x);
          this->object_stack.push_back(x->methods["__unary_not"]);
          this->call_function();
          break;
        }

        this->object_stack.push_back(result);
        object_heap.push_back(result);
        this->tick_gc();

        break;
      }
      case instructions::ret: {
        while (this->get_current_frame()->scope_type != block_type::function) {
          this->stack_frame_stack.pop_back();
        };
        if (this->get_current_frame()->scope_type != block_type::global)
          this->stack_frame_stack.pop_back();
        this->ip = this->return_ip_stack.back();
        this->return_ip_stack.pop_back();
        break;
      }
      case instructions::close: {
        while (this->get_current_frame()->scope_type != block_type::loop) {
          this->stack_frame_stack.pop_back();
        };
        if (this->get_current_frame()->scope_type != block_type::global)
          this->stack_frame_stack.pop_back();
        break;
      }
      case instructions::skip: {
        while (this->get_current_frame()->scope_type != block_type::loop) {
          this->stack_frame_stack.pop_back();
        };
        break;
      }
      case instructions::make_function: {
        uint64_t ip = this->read<uint64_t>();
        uint8_t up_values = this->read<uint8_t>();
        function_object* function = new function_object;
        gem_function* gem_function_value = new gem_function;
        gem_function_value->ip = ip;
        function->value_type = value_types::function;
        function->function_type = function_types::gem_function;
        function->gem_function_value = gem_function_value;

        stack_frame* frame = this->get_current_frame();

        for (uint8_t up_value_index = 0; up_value_index < up_values;
             ++up_value_index) {
          bool is_local = this->read<uint8_t>();
          if (is_local) {
            gem_function_value->up_values.push_back(
                &frame->local_values[this->read<uint8_t>()]);
          } else {
            gem_function_value->up_values.push_back(
                frame->up_values[this->read<uint8_t>()]);
          }
        }

        this->object_stack.push_back(function);
        object_heap.push_back(function);
        this->tick_gc();
        break;
      }
      case instructions::call: {
        this->call_function();
        break;
      }
      case instructions::halt: {
        this->halted = true;
        break;
      }
      case instructions::new_table: {
        table* table_value = new table;
        table_object* obj = new table_object(table_value);
        obj->value_type = value_types::table;

        this->object_stack.push_back(obj);
        object_heap.push_back(obj);
        this->tick_gc();
        break;
      }
      case instructions::store_array: {
        object* value = this->pop();
        number_object* index_obj = static_cast<number_object*>(this->pop());
        table_object* table_obj = static_cast<table_object*>(this->pop());
        uint64_t index = index_obj->value;

        if (table_obj->value->array.size() < index + 1) {
          table_obj->value->array.resize(index + 1);
        }
        table_obj->value->array[index] = value;
        break;
      }
      case instructions::get_array: {
        number_object* index = static_cast<number_object*>(this->pop());
        object* parent = this->pop();

        if (parent->value_type != value_types::class_) {
          table_object* table_obj = static_cast<table_object*>(parent);

          this->object_stack.push_back(
              table_obj->value->array[static_cast<uint64_t>(index->value)]);
        } else {
          this->object_stack.push_back(parent);
          this->object_stack.push_back(index);
          this->object_stack.push_back(parent->methods["__indexitem"]);
          this->call_function();
        }

        break;
      }
      case instructions::store_hash: {
        object* value = this->pop();
        object* key = this->pop();
        table_object* table_obj = static_cast<table_object*>(this->pop());

        table_obj->value->dictionary[key->hash()] = value;
        break;
      }
      case instructions::get_hash: {
        object* key = this->pop();
        object* parent = this->pop();

        if (parent->value_type != value_types::class_) {
          table_object* table_obj = static_cast<table_object*>(parent);
          this->object_stack.push_back(
              table_obj->value->dictionary[key->hash()]);
        } else {
          this->object_stack.push_back(parent);
          this->object_stack.push_back(key);
          this->object_stack.push_back(parent->methods["__getmember"]);
          this->call_function();
        }

        break;
      }
      case instructions::load_global: {
        uint8_t index = this->read<uint8_t>();
        object* global = this->globals[index];

        this->object_stack.push_back(global);
        object_heap.push_back(global);
        this->tick_gc();
        break;
      }
      case instructions::define_class: {
        class_object* class_obj = new class_object();
        class_obj->value_type = value_types::class_;

        this->object_stack.push_back(class_obj);
        object_heap.push_back(class_obj);
        this->tick_gc();
        if (class_obj->methods.find("__init__") != class_obj->methods.end()) {
          this->object_stack.push_back(class_obj->methods["__init__"]);
          this->call_function();
        };
        break;
      }
      case instructions::get_attr: {
        string_object* attr = static_cast<string_object*>(this->pop());
        object* self = this->pop();

        if (self->methods.find(attr->value) != self->methods.end())
          this->object_stack.push_back(self);
        break;
      }
      case instructions::store_method: {
        object* value = this->pop();
        string_object* method = static_cast<string_object*>(this->pop());
        class_object* class_obj =
            static_cast<class_object*>(this->object_stack.back());

        class_obj->methods[method->value] = value;
        break;
      }
      case instructions::print_str: {
        std::string value;

        while (this->ip < program_size && bytes[this->ip] != 0) {
          value += this->read<char>();
        }
        this->read<uint8_t>();
        if (value == "<state>") {
          std::cout << "-----------------" << std::endl;
          for (object* obj : this->object_stack) {
            obj->print();
          }
          std::cout << "-----------------" << std::endl;
        } else {
          std::cout << value << std::endl;
        }
        break;
      }
      case instructions::pop: {
        this->pop();
        break;
      }
      default:
        std::cerr << "Invalid instruction!\n";
        this->exit_handler(1);
        break;
    }
  }
}

void GEM_VIRTUAL_MACHINE::tick_gc() {
  this->allocations++;
  if (this->allocations >= gc_trigger_allocation_count) {
    this->allocations = 0;
    this->garbage_collect();
  }
}

void GEM_VIRTUAL_MACHINE::garbage_collect() {
  for (stack_frame* frame : this->stack_frame_stack) {
    frame->marked = true;
    for (object* obj : frame->local_values) obj->mark();
    for (object** obj : frame->up_values) (*obj)->mark();
  }

  for (object* obj : this->object_stack) obj->mark();
  for (object* obj : this->globals) obj->mark();

  std::vector<stack_frame*> new_stack_frame_heap;
  for (stack_frame* frame : this->stack_frame_stack) {
    if (!frame->marked) {
      delete frame;
    } else {
      new_stack_frame_heap.push_back(frame);
    }
  }

  std::vector<object*> new_obj_heap;
  for (object* obj : object_heap) {
    if (!obj->marked) {
      obj->free();
      switch (obj->value_type) {
        case value_types::number:
          delete static_cast<number_object*>(obj);
          break;
        case value_types::string:
          delete static_cast<string_object*>(obj);
          break;
        case value_types::boolean:
          delete static_cast<boolean_object*>(obj);
          break;
        case value_types::function:
          delete static_cast<function_object*>(obj);
          break;
        case value_types::null:
          delete static_cast<null_object*>(obj);
          break;
        default:
          delete obj;
      }
    } else {
      obj->marked = false;
      new_obj_heap.push_back(obj);
    }
  }

  object_heap = new_obj_heap;
}

int main(int argc, char* argv[]) {
  std::ifstream input(argv[1], std::ios::in | std::ios::binary);
  std::vector<uint8_t> buffer;
  input.seekg(0, std::ios::end);
  std::streampos size = input.tellg();
  buffer.resize(size);
  input.seekg(0, std::ios::beg);
  input.read(reinterpret_cast<char*>(buffer.data()), size);
  input.close();

  GEM_VIRTUAL_MACHINE* vm = new GEM_VIRTUAL_MACHINE();
  vm->bytes = buffer;
  stack_frame* global = new stack_frame;
  global->scope_type = block_type::global;
  vm->stack_frame_stack.push_back(global);
  vm->execute();
  vm->exit_handler(0);

  return 0;
}