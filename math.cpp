#include <iostream>

#include "src/object.hpp"

extern "C" {
object* add(number_object* a, number_object* b) { return a->add(b); }
}