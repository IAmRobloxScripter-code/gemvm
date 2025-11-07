#include "genlib.hpp"
#include <fstream>

int main() {
    bytecode bytes;
    bytes.write_header();
    bytes.write_make_constant<bool>(value_types::boolean, true);
    bytes.write_make_constant<bool>(value_types::boolean, false);
    bytes.write_make_frame(0, block_type::global);
    bytes.write_load_constant(0);
    bytes.write_load_constant(1);
    bytes.write_cmp_eq();
    bytes.write_jmp_if_true("if_1");
    bytes.write_load_constant(0);
    bytes.write_load_constant(1);
    bytes.write_cmp_eq();
    bytes.write_jmp_if_false("else_1");
    bytes.write_label("if_1");
    bytes.write_load_constant(0);
    bytes.write_jmp("if_end_1");
    bytes.write_label("else_1");
    bytes.write_jmp("if_end_1");
    bytes.write_label("if_end_1");
    bytes.write_halt();
    bytes.process_bytecode();

    auto bytecode_array = bytes.get_bytecode();
    std::ofstream output("output", std::ios::out | std::ios::binary);

    for (uint8_t byte : bytecode_array) {
        output.write(reinterpret_cast<const char*>(&byte), sizeof(uint8_t));
    }

    output.close();
    return 0;
};