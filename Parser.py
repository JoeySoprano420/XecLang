class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.position = 0
        self.current_token = self.tokens[0] if self.tokens else None

    def consume(self):
        """Advance to the next token"""
        self.position += 1
        if self.position < len(self.tokens):
            self.current_token = self.tokens[self.position]
        else:
            self.current_token = None

    def match(self, token_type):
        """Check if the current token matches the expected token type"""
        if self.current_token and self.current_token[0] == token_type:
            self.consume()
            return True
        return False

    def parse(self):
        """Start parsing the entire program"""
        return self.program()

    def program(self):
        """Program: Can be a list of statements"""
        statements = []
        while self.current_token:
            statements.append(self.statement())
        return {'type': 'Program', 'body': statements}

    def statement(self):
        """A statement can be a declaration, assignment, or control flow"""
        if self.match('KEYWORD') and self.current_token[1] == 'function':
            return self.function_declaration()
        elif self.match('KEYWORD') and self.current_token[1] == 'class':
            return self.class_declaration()
        elif self.match('IDENTIFIER'):
            return self.assignment()
        elif self.match('KEYWORD') and self.current_token[1] == 'if':
            return self.if_statement()
        elif self.match('KEYWORD') and self.current_token[1] == 'while':
            return self.while_loop()
        elif self.match('KEYWORD') and self.current_token[1] == 'for':
            return self.for_loop()
        elif self.match('KEYWORD') and self.current_token[1] == 'return':
            return self.return_statement()
        else:
            raise SyntaxError(f"Unexpected token: {self.current_token}")

    def function_declaration(self):
        """Parse function declarations"""
        self.match('KEYWORD')  # 'function'
        name = self.match('IDENTIFIER')
        self.match('DELIMITER')  # '('
        params = self.parameters()
        self.match('DELIMITER')  # ')'
        self.match('DELIMITER')  # '{'
        body = self.block()
        self.match('DELIMITER')  # '}'
        return {'type': 'FunctionDeclaration', 'name': name, 'params': params, 'body': body}

    def class_declaration(self):
        """Parse class declaration"""
        self.match('KEYWORD')  # 'class'
        name = self.match('IDENTIFIER')
        self.match('DELIMITER')  # '{'
        members = self.class_members()
        self.match('DELIMITER')  # '}'
        return {'type': 'ClassDeclaration', 'name': name, 'members': members}

    def class_members(self):
        """Handle class members such as variables and functions"""
        members = []
        while self.current_token and self.current_token[0] != 'DELIMITER' and self.current_token[1] != '}':
            if self.match('DATA_TYPE'):
                var_name = self.match('IDENTIFIER')
                self.match('ASSIGNMENT')
                value = self.expression()
                members.append({'type': 'Variable', 'name': var_name, 'value': value})
            elif self.match('KEYWORD') and self.current_token[1] == 'function':
                members.append(self.function_declaration())
        return members

    def parameters(self):
        """Handle function parameters (if any)"""
        params = []
        while self.current_token and self.current_token[0] != 'DELIMITER' and self.current_token[1] != ')':
            if self.match('IDENTIFIER'):
                params.append(self.current_token[1])
                if not self.match('DELIMITER'):  # Ensure we are on a comma or closing parenthesis
                    break
        return params

    def assignment(self):
        """Handle variable assignments"""
        var_name = self.current_token[1]
        self.match('IDENTIFIER')  # The variable name
        self.match('ASSIGNMENT')  # '=' sign
        expr = self.expression()  # Expression on the right-hand side
        return {'type': 'Assignment', 'name': var_name, 'value': expr}

    def if_statement(self):
        """Parse an if-else conditional statement"""
        self.match('KEYWORD')  # 'if'
        self.match('DELIMITER')  # '('
        condition = self.expression()
        self.match('DELIMITER')  # ')'
        self.match('DELIMITER')  # '{'
        body = self.block()
        self.match('DELIMITER')  # '}'

        else_body = None
        if self.match('KEYWORD') and self.current_token[1] == 'else':
            self.match('DELIMITER')  # '{'
            else_body = self.block()
            self.match('DELIMITER')  # '}'

        return {'type': 'IfStatement', 'condition': condition, 'body': body, 'else': else_body}

    def while_loop(self):
        """Parse while loop"""
        self.match('KEYWORD')  # 'while'
        self.match('DELIMITER')  # '('
        condition = self.expression()
        self.match('DELIMITER')  # ')'
        self.match('DELIMITER')  # '{'
        body = self.block()
        self.match('DELIMITER')  # '}'
        return {'type': 'WhileLoop', 'condition': condition, 'body': body}

    def for_loop(self):
        """Parse for loop"""
        self.match('KEYWORD')  # 'for'
        self.match('DELIMITER')  # '('
        init = self.assignment()  # The initialization (e.g., int i = 0)
        condition = self.expression()  # The loop condition (e.g., i < 10)
        self.match('DELIMITER')  # ';'
        increment = self.expression()  # The increment (e.g., i++)
        self.match('DELIMITER')  # ')'
        self.match('DELIMITER')  # '{'
        body = self.block()
        self.match('DELIMITER')  # '}'
        return {'type': 'ForLoop', 'init': init, 'condition': condition, 'increment': increment, 'body': body}

    def return_statement(self):
        """Parse return statement"""
        self.match('KEYWORD')  # 'return'
        value = self.expression()
        self.match('DELIMITER')  # ';'
        return {'type': 'ReturnStatement', 'value': value}

    def block(self):
        """Block is a list of statements enclosed in curly braces"""
        statements = []
        while self.current_token and self.current_token[0] != 'DELIMITER' and self.current_token[1] != '}':
            statements.append(self.statement())
        return statements

    def expression(self):
        """Handle expressions: numbers, identifiers, operators, etc."""
        # For simplicity, this handles basic binary operations (you can expand for more)
        left = self.term()
        while self.current_token and self.current_token[0] == 'OPERATOR':
            operator = self.current_token[1]
            self.consume()
            right = self.term()
            left = {'type': 'BinaryExpression', 'left': left, 'operator': operator, 'right': right}
        return left

    def term(self):
        """Handle basic terms such as literals or identifiers"""
        if self.match('NUMBER'):
            return {'type': 'Literal', 'value': int(self.current_token[1])}
        elif self.match('FLOAT'):
            return {'type': 'Literal', 'value': float(self.current_token[1])}
        elif self.match('STRING'):
            return {'type': 'Literal', 'value': self.current_token[1][1:-1]}  # Strip quotes
        elif self.match('IDENTIFIER'):
            return {'type': 'Identifier', 'name': self.current_token[1]}
        elif self.match('BOOLEAN'):
            return {'type': 'Literal', 'value': self.current_token[1] == 'true'}
        elif self.match('ARRAY_START'):
            return self.array()
        elif self.match('TUPLE_START'):
            return self.tuple()
        elif self.match('DELIMITER') and self.current_token[1] == '(':
            expr = self.expression()
            self.match('DELIMITER')  # ')'
            return expr
        else:
            raise SyntaxError(f"Unexpected token in expression: {self.current_token}")

    def array(self):
        """Parse an array expression"""
        elements = []
        while self.current_token and self.current_token[0] != 'ARRAY_END':
            elements.append(self.expression())
            if self.match('DELIMITER') and self.current_token[1] == ',':
                continue
            break
        self.match('ARRAY_END')  # Closing bracket
        return {'type': 'Array', 'elements': elements}

    def tuple(self):
        """Parse a tuple expression"""
        elements = []
        while self.current_token and self.current_token[0] != 'TUPLE_END':
            elements.append(self.expression())
            if self.match('DELIMITER') and self.current_token[1] == ',':
                continue
            break
        self.match('TUPLE_END')  # Closing parenthesis
        return {'type': 'Tuple', 'elements': elements}

