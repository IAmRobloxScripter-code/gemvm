#pragma once
#include <cstdint>

enum class instructions : uint8_t
{
    push,
    make_constant,
    load_constant,

    store_local,
    load_local,

    make_frame,
    delete_frame,

    ret,

    jmp,
    jmp_if_true,
    jmp_if_false,

    cmp_eq,
    cmp_neq,
    cmp_gt,
    cmp_lt,
    cmp_gte,
    cmp_lte,
    unary_not,

    and_check,
    or_check,

    add,
    sub,
    mul,
    div,
    pow,
    mod,

    make_function,
    call,
    halt,
    
    close,
    skip
};