class CodeGenerator:
    def __init__(self):
        self.code = []
        self.current_scope = {}  # To track variables and functions in scope

    def generate(self, node):
        """Main function to generate code from AST nodes"""
        if node['type'] == 'Program':
            self.program(node)
        elif node['type'] == 'FunctionDeclaration':
            self.function_declaration(node)
        elif node['type'] == 'ClassDeclaration':
            self.class_declaration(node)
        elif node['type'] == 'Assignment':
            self.assignment(node)
        elif node['type'] == 'IfStatement':
            self.if_statement(node)
        elif node['type'] == 'WhileLoop':
            self.while_loop(node)
        elif node['type'] == 'ForLoop':
            self.for_loop(node)
        elif node['type'] == 'ReturnStatement':
            self.return_statement(node)
        elif node['type'] == 'TryCatchStatement':
            self.try_catch(node)
        elif node['type'] == 'Literal':
            self.literal(node)
        elif node['type'] == 'BinaryExpression':
            self.binary_expression(node)
        elif node['type'] == 'Identifier':
            self.identifier(node)
        elif node['type'] == 'Array':
            self.array(node)
        elif node['type'] == 'Tuple':
            self.tuple(node)
        elif node['type'] == 'Enum':
            self.enum(node)
        elif node['type'] == 'Struct':
            self.struct(node)
        else:
            raise ValueError(f"Unknown node type: {node['type']}")

    def program(self, node):
        """Generate code for the program (top-level block)"""
        for statement in node['body']:
            self.generate(statement)
        return '\n'.join(self.code)

    def function_declaration(self, node):
        """Generate code for function declaration with type checking"""
        params = ', '.join([f"{param['name']}:{param['type']}" for param in node['params']])
        self.code.append(f"function {node['name']}({params}) {{")
        
        # Enter function scope to track local variables
        self.current_scope = {param['name']: param['type'] for param in node['params']}
        
        for stmt in node['body']:
            self.generate(stmt)
        
        self.code.append("}")

    def class_declaration(self, node):
        """Generate code for class declaration"""
        self.code.append(f"class {node['name']} {{")
        for member in node['members']:
            self.generate(member)
        self.code.append("}")

    def assignment(self, node):
        """Generate code for variable assignment with type checking"""
        if node['name'] not in self.current_scope:
            raise ValueError(f"Variable '{node['name']}' not declared in current scope")
        
        # Check if the type matches the declared type
        expected_type = self.current_scope[node['name']]
        value_type = self.generate_expression(node['value']).split()[0]
        if expected_type != value_type:
            raise TypeError(f"Type mismatch: expected '{expected_type}', got '{value_type}'")
        
        self.code.append(f"{node['name']} = {self.generate_expression(node['value'])};")

    def if_statement(self, node):
        """Generate code for if statement"""
        self.code.append(f"if ({self.generate_expression(node['condition'])}) {{")
        for stmt in node['body']:
            self.generate(stmt)
        self.code.append("}")
        
        if node['else']:
            self.code.append("else {")
            for stmt in node['else']:
                self.generate(stmt)
            self.code.append("}")

    def while_loop(self, node):
        """Generate code for while loop"""
        self.code.append(f"while ({self.generate_expression(node['condition'])}) {{")
        for stmt in node['body']:
            self.generate(stmt)
        self.code.append("}")

    def for_loop(self, node):
        """Generate code for for loop"""
        self.code.append(f"for ({self.generate_expression(node['init'])}; "
                         f"{self.generate_expression(node['condition'])}; "
                         f"{self.generate_expression(node['increment'])}) {{")
        for stmt in node['body']:
            self.generate(stmt)
        self.code.append("}")

    def return_statement(self, node):
        """Generate code for return statement"""
        self.code.append(f"return {self.generate_expression(node['value'])};")

    def try_catch(self, node):
        """Generate code for try-catch block"""
        self.code.append("try {")
        for stmt in node['try_block']:
            self.generate(stmt)
        self.code.append("} catch (error) {")
        for stmt in node['catch_block']:
            self.generate(stmt)
        self.code.append("}")

    def literal(self, node):
        """Generate code for literal values (numbers, strings, booleans)"""
        self.code.append(str(node['value']))

    def binary_expression(self, node):
        """Generate code for binary expressions (e.g., a + b, x == 5)"""
        left = self.generate_expression(node['left'])
        right = self.generate_expression(node['right'])
        self.code.append(f"{left} {node['operator']} {right}")

    def identifier(self, node):
        """Generate code for identifiers"""
        self.code.append(node['name'])

    def array(self, node):
        """Generate code for array expressions"""
        elements = [self.generate_expression(e) for e in node['elements']]
        self.code.append(f"[{', '.join(elements)}]")

    def tuple(self, node):
        """Generate code for tuple expressions"""
        elements = [self.generate_expression(e) for e in node['elements']]
        self.code.append(f"({', '.join(elements)})")

    def enum(self, node):
        """Generate code for enums"""
        self.code.append(f"enum {node['name']} {{")
        for value in node['values']:
            self.code.append(f"    {value},")
        self.code.append("}")

    def struct(self, node):
        """Generate code for structs"""
        self.code.append(f"struct {node['name']} {{")
        for field in node['fields']:
            self.code.append(f"    {field['name']} : {field['type']};")
        self.code.append("}")

    def generate_expression(self, expr):
        """Helper function to handle different expression types"""
        if expr['type'] == 'Literal':
            return str(expr['value'])
        elif expr['type'] == 'BinaryExpression':
            return f"({self.generate_expression(expr['left'])} {expr['operator']} {self.generate_expression(expr['right'])})"
        elif expr['type'] == 'Identifier':
            return expr['name']
        elif expr['type'] == 'Array':
            return f"[{', '.join([self.generate_expression(e) for e in expr['elements']])}]"
        elif expr['type'] == 'Tuple':
            return f"({', '.join([self.generate_expression(e) for e in expr['elements']])})"
        else:
            raise ValueError(f"Unsupported expression type: {expr['type']}")

