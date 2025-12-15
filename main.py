import ast

code = """
def foo():
    return 100;

x = foo()
"""
tree = ast.parse(code)
print(ast.dump(tree, indent=4))
