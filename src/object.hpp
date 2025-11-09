#pragma once
#include <string>
#include <vector>
#include <cstdint>

enum class value_types : uint8_t
{
    number,
    string,
    boolean,
    table,
    null,
    function
};

struct object
{
    bool marked = false;
    value_types value_type;

    virtual void mark();
    virtual void free();
    virtual void print();

    virtual object *add(object *other);
    virtual object *sub(object *other);
    virtual object *mul(object *other);
    virtual object *div(object *other);
    virtual object *powr(object *other);
    virtual object *modl(object *other);
    virtual object *cmp_eq(object *other);
    virtual object *cmp_neq(object *other);
    virtual object *cmp_gt(object *other);
    virtual object *cmp_lt(object *other);
    virtual object *cmp_gte(object *other);
    virtual object *cmp_lte(object *other);
    virtual object *unary_not(object *other);
    virtual object *copy();
    virtual ~object() = default;
};

struct number_object : public object
{
    double value;
    number_object(double value);

    void mark() override;
    void free() override;
    void print() override;

    object *add(object *other) override;
    object *sub(object *other) override;
    object *mul(object *other) override;
    object *div(object *other) override;
    object *powr(object *other) override;
    object *modl(object *other) override;
    object *cmp_eq(object *other) override;
    object *cmp_neq(object *other) override;
    object *cmp_gt(object *other) override;
    object *cmp_lt(object *other) override;
    object *cmp_gte(object *other) override;
    object *cmp_lte(object *other) override;
    object *unary_not(object *other) override;
    virtual object *copy();
};

struct string_object : public object
{
    std::string value;
    string_object(std::string value);

    void mark() override;
    void free() override;
    void print() override;

    object *add(object *other) override;
    object *sub(object *other) override;
    object *mul(object *other) override;
    object *div(object *other) override;
    object *powr(object *other) override;
    object *modl(object *other) override;
    object *cmp_eq(object *other) override;
    object *cmp_neq(object *other) override;
    object *cmp_gt(object *other) override;
    object *cmp_lt(object *other) override;
    object *cmp_gte(object *other) override;
    object *cmp_lte(object *other) override;
    object *unary_not(object *other) override;
    virtual object *copy();
};

struct boolean_object : public object
{
    bool value;
    boolean_object(bool value);

    void mark() override;
    void free() override;
    void print() override;

    object *add(object *other) override;
    object *sub(object *other) override;
    object *mul(object *other) override;
    object *div(object *other) override;
    object *powr(object *other) override;
    object *modl(object *other) override;
    object *cmp_eq(object *other) override;
    object *cmp_neq(object *other) override;
    object *cmp_gt(object *other) override;
    object *cmp_lt(object *other) override;
    object *cmp_gte(object *other) override;
    object *cmp_lte(object *other) override;
    object *unary_not(object *other) override;
    virtual object *copy();
};

struct null_object : public object
{
    null_object();

    void mark() override;
    void free() override;
    void print() override;

    object *add(object *other) override;
    object *sub(object *other) override;
    object *mul(object *other) override;
    object *div(object *other) override;
    object *powr(object *other) override;
    object *modl(object *other) override;
    object *cmp_eq(object *other) override;
    object *cmp_neq(object *other) override;
    object *cmp_gt(object *other) override;
    object *cmp_lt(object *other) override;
    object *cmp_gte(object *other) override;
    object *cmp_lte(object *other) override;
    object *unary_not(object *other) override;
    virtual object *copy();
};

struct function_object : public object
{
    std::vector<object **> up_values;
    uint64_t ip;

    function_object();

    void mark() override;
    void free() override;
    void print() override;

    object *add(object *other) override;
    object *sub(object *other) override;
    object *mul(object *other) override;
    object *div(object *other) override;
    object *powr(object *other) override;
    object *modl(object *other) override;
    object *cmp_eq(object *other) override;
    object *cmp_neq(object *other) override;
    object *cmp_gt(object *other) override;
    object *cmp_lt(object *other) override;
    object *cmp_gte(object *other) override;
    object *cmp_lte(object *other) override;
    object *unary_not(object *other) override;
    virtual object *copy();
};