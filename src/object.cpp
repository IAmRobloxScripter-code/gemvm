#include "object.hpp"
#include <cmath>
#include <iostream>

number_object::number_object(double value)
{
    this->value = value;
}

string_object::string_object(std::string value)
{
    this->value = value;
}

boolean_object::boolean_object(bool value)
{
    this->value = value;
}

null_object::null_object() = default;
function_object::function_object() = default;

object *object::add(object *other) { return nullptr; };
object *object::sub(object *other) { return nullptr; };
object *object::mul(object *other) { return nullptr; };
object *object::div(object *other) { return nullptr; };
object *object::powr(object *other) { return nullptr; };
object *object::modl(object *other) { return nullptr; };
object *object::cmp_eq(object *other) { return nullptr; };
object *object::cmp_neq(object *other) { return nullptr; };
object *object::cmp_gt(object *other) { return nullptr; };
object *object::cmp_lt(object *other) { return nullptr; };
object *object::cmp_gte(object *other) { return nullptr; };
object *object::cmp_lte(object *other) { return nullptr; };
object *object::unary_not(object *other) { return nullptr; };

object *number_object::add(object *other)
{
    double value = this->value + static_cast<number_object *>(other)->value;
    object *result = new number_object(value);
    result->value_type = value_types::number;
    return result;
}

object *number_object::sub(object *other)
{
    double value = this->value - static_cast<number_object *>(other)->value;
    number_object *result = new number_object(value);
    result->value_type = value_types::number;
    return result;
}

object *number_object::mul(object *other)
{
    double value = this->value * static_cast<number_object *>(other)->value;
    number_object *result = new number_object(value);
    result->value_type = value_types::number;
    return result;
}

object *number_object::div(object *other)
{
    double value = this->value / static_cast<number_object *>(other)->value;
    number_object *result = new number_object(value);
    result->value_type = value_types::number;
    return result;
}

object *number_object::powr(object *other)
{
    double value = pow(this->value, static_cast<number_object *>(other)->value);
    number_object *result = new number_object(value);
    result->value_type = value_types::number;
    return result;
}

object *number_object::modl(object *other)
{
    double value = fmod(this->value, static_cast<number_object *>(other)->value);
    number_object *result = new number_object(value);
    result->value_type = value_types::number;
    return result;
}

object *number_object::cmp_eq(object *other)
{
    bool value = this->value == static_cast<number_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *number_object::cmp_neq(object *other)
{
    bool value = this->value != static_cast<number_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *number_object::cmp_gt(object *other)
{
    bool value = this->value > static_cast<number_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *number_object::cmp_lt(object *other)
{
    bool value = this->value < static_cast<number_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *number_object::cmp_gte(object *other)
{
    bool value = this->value >= static_cast<number_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *number_object::cmp_lte(object *other)
{
    bool value = this->value <= static_cast<number_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *number_object::unary_not(object *other)
{
    bool value = !this->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *string_object::add(object *other)
{
    std::string value = this->value + static_cast<string_object *>(other)->value;
    string_object *result = new string_object(value);
    result->value_type = value_types::string;
    return result;
}

object *string_object::sub(object *other)
{
    return nullptr;
}

object *string_object::mul(object *other)
{
    return nullptr;
}

object *string_object::div(object *other)
{
    return nullptr;
}

object *string_object::powr(object *other)
{
    return nullptr;
}

object *string_object::modl(object *other)
{
    return nullptr;
}

object *string_object::cmp_eq(object *other)
{
    bool value = this->value == static_cast<string_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *string_object::cmp_neq(object *other)
{
    bool value = this->value != static_cast<string_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *string_object::cmp_gt(object *other)
{
    return nullptr;
}

object *string_object::cmp_lt(object *other)
{
    return nullptr;
}

object *string_object::cmp_gte(object *other)
{
    return nullptr;
}

object *string_object::cmp_lte(object *other)
{
    return nullptr;
}

object *string_object::unary_not(object *other)
{
    bool value = !(this->value.size() == 0 ? false : true);
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *boolean_object::add(object *other) { return nullptr; };
object *boolean_object::sub(object *other) { return nullptr; };
object *boolean_object::mul(object *other) { return nullptr; };
object *boolean_object::div(object *other) { return nullptr; };
object *boolean_object::powr(object *other) { return nullptr; };
object *boolean_object::modl(object *other) { return nullptr; };
object *boolean_object::cmp_eq(object *other)
{
    bool value = this->value == static_cast<boolean_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
};

object *boolean_object::cmp_neq(object *other)
{
    bool value = this->value != static_cast<boolean_object *>(other)->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
};

object *boolean_object::cmp_gt(object *other) { return nullptr; };
object *boolean_object::cmp_lt(object *other) { return nullptr; };
object *boolean_object::cmp_gte(object *other) { return nullptr; };
object *boolean_object::cmp_lte(object *other) { return nullptr; };
object *boolean_object::unary_not(object *other)
{
    bool value = !this->value;
    boolean_object *result = new boolean_object(value);
    result->value_type = value_types::boolean;
    return result;
}

object *null_object::add(object *other) { return nullptr; };
object *null_object::sub(object *other) { return nullptr; };
object *null_object::mul(object *other) { return nullptr; };
object *null_object::div(object *other) { return nullptr; };
object *null_object::powr(object *other) { return nullptr; };
object *null_object::modl(object *other) { return nullptr; };
object *null_object::cmp_eq(object *other) { return nullptr; };
object *null_object::cmp_neq(object *other) { return nullptr; };
object *null_object::cmp_gt(object *other) { return nullptr; };
object *null_object::cmp_lt(object *other) { return nullptr; };
object *null_object::cmp_gte(object *other) { return nullptr; };
object *null_object::cmp_lte(object *other) { return nullptr; };
object *null_object::unary_not(object *other) { return nullptr; };

object *function_object::add(object *other) { return nullptr; };
object *function_object::sub(object *other) { return nullptr; };
object *function_object::mul(object *other) { return nullptr; };
object *function_object::div(object *other) { return nullptr; };
object *function_object::powr(object *other) { return nullptr; };
object *function_object::modl(object *other) { return nullptr; };
object *function_object::cmp_eq(object *other) { return nullptr; };
object *function_object::cmp_neq(object *other) { return nullptr; };
object *function_object::cmp_gt(object *other) { return nullptr; };
object *function_object::cmp_lt(object *other) { return nullptr; };
object *function_object::cmp_gte(object *other) { return nullptr; };
object *function_object::cmp_lte(object *other) { return nullptr; };
object *function_object::unary_not(object *other) { return nullptr; };

void object::free() {};
void object::mark() { this->marked = true; };

void number_object::free() {};
void string_object::free()
{
    this->value.~basic_string();
};
void boolean_object::free() {};
void null_object::free() {};
void function_object::free() {};

void number_object::mark() { this->marked = true; };
void string_object::mark() { this->marked = true; };
void boolean_object::mark() { this->marked = true; };
void null_object::mark() { this->marked = true; };
void function_object::mark() { this->marked = true; };

void object::print() { std::cout << "object" << std::endl; };
void number_object::print() { std::cout << this->value << std::endl; };
void string_object::print() { std::cout << this->value << std::endl; };
void boolean_object::print() { std::cout << this->value << std::endl; };
void null_object::print() { std::cout << "null" << std::endl; };
void function_object::print() { std::cout << "function" << std::endl; };

object *object::copy() { return nullptr; };
object *number_object::copy()
{
    number_object *obj = new number_object(this->value);
    obj->value_type = this->value_type;
    return obj;
}
object *string_object::copy()
{
    string_object *obj = new string_object(this->value);
    obj->value_type = this->value_type;
    return obj;
}
object *boolean_object::copy()
{
    boolean_object *obj = new boolean_object(this->value);
    obj->value_type = this->value_type;
    return obj;
}
object *null_object::copy()
{
    null_object *obj = new null_object();
    obj->value_type = this->value_type;
    return obj;
}
object *function_object::copy()
{
    function_object *obj = new function_object();
    obj->value_type = this->value_type;
    obj->ip = this->ip;
    obj->up_values = this->up_values;
    return obj;
}
