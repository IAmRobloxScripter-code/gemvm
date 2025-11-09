#pragma once
#include "stack_frame.hpp"
#include "instructions.hpp"
#include <cstring>
#include <iostream>

#define call_stack_limit 64
#define gvm_major 0
#define gvm_minor 1
#define header_size sizeof(char) * 3 + sizeof(uint64_t)
#define gc_trigger_allocation_count 250

static std::vector<object *> object_heap;
static std::vector<stack_frame *> stack_frame_heap;

class GEM_VIRTUAL_MACHINE
{
public:
    std::vector<object *> object_stack;
    std::vector<stack_frame *> stack_frame_stack;
    std::vector<uint64_t> return_ip_stack;
    std::vector<object *> constants;
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
    object *pop()
    {
        object *value = this->object_stack.back();
        this->object_stack.pop_back();
        return value;
    };
    stack_frame *get_current_frame()
    {
        if (this->stack_frame_stack.size() == 0) {
            std::cerr << "Stack underflow!\n";
            this->exit_handler(1);
            return nullptr;
        }
        return this->stack_frame_stack.back();
    };
    template <typename T>
    T read()
    {
        T value;
        std::memcpy(&value, &this->bytes[this->ip], sizeof(T));
        this->ip += sizeof(T);
        return value;
    }
};
