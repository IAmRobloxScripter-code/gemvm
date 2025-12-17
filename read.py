import marshal
import dis
import sys

# Path to the .pyc file
pyc_file = "../__pycache__/main.cpython-313.pyc"

with open(pyc_file, "rb") as f:
    f.read(16)  # skip header
    code_obj = marshal.load(f)

# Disassemble the code object with debug info
disassembly = dis.Bytecode(code_obj)
output_file = "myscript_bytecode.txt"

with open(output_file, "a") as out:
    out.write("\nConstants:\n")
    for i, c in enumerate(code_obj.co_consts):
        out.write(f"{i}: {repr(c)}\n")

    out.write("\nLocals:\n")
    for i, name in enumerate(code_obj.co_varnames):
        out.write(f"{i}: {name}\n")

    out.write("\nFreevars:\n")
    for name in code_obj.co_freevars:
        out.write(f"{name}\n")

    out.write("\nCellvars:\n")
    for name in code_obj.co_cellvars:
        out.write(f"{name}\n")

out.close()
