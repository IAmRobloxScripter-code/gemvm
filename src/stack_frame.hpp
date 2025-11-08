#pragma once
#include "object.hpp"

enum class block_type : uint8_t {
    global,
    function,
    loop,
    conditional,
};

struct stack_frame
{   
    block_type scope_type;
    bool marked = false;
    std::vector<object*> up_values;
    std::vector<object*> local_values;
};
