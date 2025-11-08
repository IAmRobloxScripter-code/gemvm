#include "vm.hpp"
#include <iostream>
#include <fstream>

void GEM_VIRTUAL_MACHINE::exit_handler(uint8_t exit_code)
{   
    for (object *obj : this->object_stack)
    {
        obj->print();
    }

    exit(exit_code);
}

void GEM_VIRTUAL_MACHINE::is_valid_header()
{
    if (this->bytes.size() < header_size)
    {
        std::cerr << "Invalid byte size!\n Required: " << header_size << "\n Got: " << this->bytes.size() << "\n";
        this->exit_handler(1);
        return;
    }
    const uint8_t unique_id_size = header_size - sizeof(uint64_t);
    std::string unique_id;

    for (uint8_t index = 0; index < unique_id_size; ++index)
    {
        unique_id += this->read<char>();
    }

    uint64_t version = this->read<uint64_t>();

    if (unique_id != "GVM")
    {
        std::cerr << "Invalid bytecode!\n";
        this->exit_handler(1);
        return;
    }
    if (version > gvm_major * 10 + gvm_minor)
    {
        std::cerr << "Unsupported version!\n";
        this->exit_handler(1);
        return;
    }
    return;
}

void GEM_VIRTUAL_MACHINE::execute()
{
    this->is_valid_header();
    uint64_t program_size = this->bytes.size();

    while (!this->halted && this->ip < program_size)
    {
        instructions instruction = static_cast<instructions>(this->read<uint8_t>());
        switch (instruction)
        {
        case instructions::make_constant:
        {
            value_types value_type = static_cast<value_types>(this->read<uint8_t>());

            switch (value_type)
            {
            case value_types::number:
            {
                number_object *obj = new number_object(this->read<double>());
                obj->value_type = value_type;
                this->constants.push_back(obj);
                break;
            }
            case value_types::string:
            {
                std::string value;

                while (this->ip < program_size && bytes[this->ip] != 0)
                {
                    value += this->read<char>();
                }
                this->read<uint8_t>();

                string_object *obj = new string_object(value);
                obj->value_type = value_type;
                this->constants.push_back(obj);
                break;
            }
            case value_types::boolean:
            {
                boolean_object *obj = new boolean_object(this->read<uint8_t>());
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
        case instructions::load_constant:
        {
            uint32_t constant_id = this->read<u_int32_t>();
            if (this->constants.size() < constant_id)
            {
                std::cerr << "Constant does not exist!\n";
                this->exit_handler(1);
                break;
            }
            object *constant = this->constants[constant_id]->copy();

            this->object_stack.push_back(constant);
            object_heap.push_back(constant);
            break;
        }
        case instructions::make_frame:
        {
            block_type scope_type = static_cast<block_type>(this->read<uint8_t>());
            uint8_t up_value_count = this->read<uint8_t>();
            stack_frame *parent = this->get_current_frame();
            stack_frame *frame = new stack_frame;
            frame->scope_type = scope_type;

            for (uint8_t up_value_index = 0; up_value_index < up_value_count; ++up_value_count)
            {
                frame->up_values.push_back(parent->local_values[this->read<uint8_t>()]);
            }

            this->stack_frame_stack.push_back(frame);
            stack_frame_heap.push_back(frame);

            if (this->stack_frame_stack.size() > call_stack_limit)
            {
                std::cerr << "Call stack limit!\n";
                this->exit_handler(1);
                break;
            }
            break;
        }
        case instructions::delete_frame:
            this->stack_frame_stack.pop_back();
            break;
        case instructions::store_local:
        {
            object *value = this->pop();
            uint8_t id = this->read<uint8_t>();
            stack_frame *frame = this->get_current_frame();
            if (frame->local_values.size() < id + 1)
            {
                frame->local_values.resize(id + 1);
                frame->local_values[id] = value;
            }
            else
            {
                frame->local_values[id] = value;
            }
            break;
        }
        case instructions::load_local:
            this->object_stack.push_back(this->get_current_frame()->local_values[this->read<uint8_t>()]);
            break;
        case instructions::add:
        {
            object *y = this->pop();
            object *x = this->pop();

            object *result = x->add(y);
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::sub:
        {
            object *y = this->pop();
            object *x = this->pop();

            object *result = x->sub(y);
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::mul:
        {
            object *y = this->pop();
            object *x = this->pop();

            object *result = x->mul(y);
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::div:
        {
            object *y = this->pop();
            object *x = this->pop();

            object *result = x->div(y);
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::pow:
        {
            object *y = this->pop();
            object *x = this->pop();

            object *result = x->powr(y);
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::mod:
        {
            object *y = this->pop();
            object *x = this->pop();

            object *result = x->modl(y);
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }

        case instructions::jmp:
        {
            this->ip = this->read<uint64_t>();
            break;
        };
        case instructions::jmp_if_true:
        {
            boolean_object *truthy = static_cast<boolean_object *>(this->pop());
            uint64_t jmp_ip = this->read<uint64_t>();
            if (truthy->value == true)
            {
                this->ip = jmp_ip;
            }
            break;
        }
        case instructions::jmp_if_false:
        {
            boolean_object *truthy = static_cast<boolean_object *>(this->pop());
            uint64_t jmp_ip = this->read<uint64_t>();
            if (truthy->value == false)
            {
                this->ip = jmp_ip;
            }
            break;
        }
        case instructions::cmp_eq:
        {
            object *y = this->pop();
            object *x = this->pop();

            boolean_object *result = static_cast<boolean_object *>(x->cmp_eq(y));
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::cmp_neq:
        {
            object *y = this->pop();
            object *x = this->pop();

            boolean_object *result = static_cast<boolean_object *>(x->cmp_neq(y));
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::cmp_gt:
        {
            object *y = this->pop();
            object *x = this->pop();

            boolean_object *result = static_cast<boolean_object *>(x->cmp_gt(y));
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::cmp_lt:
        {
            object *y = this->pop();
            object *x = this->pop();

            boolean_object *result = static_cast<boolean_object *>(x->cmp_lt(y));
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::cmp_gte:
        {
            object *y = this->pop();
            object *x = this->pop();

            boolean_object *result = static_cast<boolean_object *>(x->cmp_gte(y));
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::cmp_lte:
        {
            object *y = this->pop();
            object *x = this->pop();

            boolean_object *result = static_cast<boolean_object *>(x->cmp_lte(y));
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::unary_not:
        {
            object *x = this->pop();

            boolean_object *result = static_cast<boolean_object *>(x->unary_not(nullptr));
            this->object_stack.push_back(result);
            object_heap.push_back(result);
            break;
        }
        case instructions::ret:
        {
            while (this->get_current_frame()->scope_type != block_type::function)
            {
                this->stack_frame_stack.pop_back();
            };
            if (this->get_current_frame()->scope_type != block_type::global)
                this->stack_frame_stack.pop_back();
            this->ip = this->return_ip_stack.back();
            this->return_ip_stack.pop_back();
            break;
        }
        case instructions::close:
        {
            while (this->get_current_frame()->scope_type != block_type::loop)
            {
                this->stack_frame_stack.pop_back();
            };
            if (this->get_current_frame()->scope_type != block_type::global)
                this->stack_frame_stack.pop_back();

            this->stack_frame_stack.pop_back();
            break;
        }
        case instructions::skip:
        {
            while (this->get_current_frame()->scope_type != block_type::loop)
            {
                this->stack_frame_stack.pop_back();
            };
            break;
        };
        case instructions::halt:
        {   
            this->halted = true;
            break;
        }
        default:
            std::cerr << "Invalid instruction!\n";
            this->exit_handler(1);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    std::ifstream input(argv[1], std::ios::in | std::ios::binary);
    std::vector<uint8_t> buffer;
    input.seekg(0, std::ios::end);
    std::streampos size = input.tellg();
    buffer.resize(size);
    input.seekg(0, std::ios::beg);
    input.read(reinterpret_cast<char *>(buffer.data()), size);
    input.close();

    GEM_VIRTUAL_MACHINE *vm = new GEM_VIRTUAL_MACHINE;
    vm->bytes = buffer;
    stack_frame* global = new stack_frame;
    global->scope_type = block_type::global;
    vm->stack_frame_stack.push_back(global);
    vm->execute();
    vm->exit_handler(0);

    return 0;
}