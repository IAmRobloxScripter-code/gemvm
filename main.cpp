#include "genlib.hpp"
#include <fstream>

int main() {
    bytecode bytes;
    bytes.write_header();
    bytes.write_make_constant<double>(value_types::number, 0);
    bytes.write_make_constant<double>(value_types::number, 10000000);
    bytes.write_make_constant<double>(value_types::number, 1);
    bytes.write_load_constant(0);
    bytes.write_store_local(0);
    bytes.write_load_constant(1);
    bytes.write_store_local(1);
    bytes.write_load_constant(2);
    bytes.write_store_local(2);
    bytes.write_make_frame(block_type::loop, std::vector<uint8_t>{0, 1, 2});
    bytes.write_label("for_loop_1");

    bytes.write_load_upvalue(0);
    bytes.write_load_upvalue(1);
    bytes.write_cmp_gt();

    bytes.write_jmp_if_true("if_1");
    bytes.write_load_upvalue(0);
    bytes.write_load_upvalue(2);
    bytes.write_add();
    bytes.write_store_upvalue(0);
    bytes.write_jmp("for_loop_1");

    bytes.write_label("if_1");
    bytes.write_close();
    bytes.write_jmp("for_loop_end_1");
    bytes.write_label("for_loop_end_1");
    bytes.write_load_local(0);
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