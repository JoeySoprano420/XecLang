import re

# Define the regex patterns for various tokens (compiled once for performance)
token_patterns = [
    ('KEYWORD', r'\b(xec|function|entry|output|runtime|pipeline|thread|layer|modify|print|if|else|elif|switch|case|return|while|for|break|continue|in|def|class|struct|inherit|and|or|not|true|false|default|loop|end)\b'),
    ('DATA_TYPE', r'\b(int|float|bool|string|byte|stream|packet|void|array|map|tuple|object)\b'),
    ('IDENTIFIER', r'[a-zA-Z_][a-zA-Z0-9_]*'),
    ('NUMBER', r'\b\d+\b'),
    ('FLOAT', r'\b\d+\.\d+\b'),
    ('STRING', r'"([^"\\]|\\.)*"'),
    ('BOOLEAN', r'\b(true|false)\b'),
    ('ARRAY_START', r'\['),
    ('ARRAY_END', r'\]'),
    ('TUPLE_START', r'\('),
    ('TUPLE_END', r'\)'),
    ('COLON', r':'),
    ('COMMA', r','),
    ('OPERATOR', r'[+\-*/%==!=<>]=|[+\-*/%<>&|=]'),
    ('ASSIGNMENT', r'='),  # Assignment operator
    ('DELIMITER', r'[{}()\[\],;:]'),
    ('COMMENT', r'//[^\n]*|/\*.*?\*/'),
    ('WHITESPACE', r'[ \t\r\n]+'),
    ('MISMATCH', r'.')  # Any single character that doesn't match the above will be treated as a mismatch
]

# Compile all patterns once for performance
compiled_patterns = [(name, re.compile(pattern)) for name, pattern in token_patterns]

class Lexer:
    def __init__(self):
        self.position = 0
        self.line = 1
        self.column = 1
        self.context = {'global': {}}  # Track global and local variables
        self.current_scope = 'global'  # Default to global scope

    def tokenize(self, code):
        tokens = []
        length = len(code)
        
        # Using re.finditer to optimize performance (returns an iterator over match objects)
        while self.position < length:
            match = None
            for token_type, compiled_regex in compiled_patterns:
                match = compiled_regex.match(code, self.position)
                if match:
                    value = match.group(0)

                    # Handle different types of tokens
                    if token_type == 'KEYWORD' and value == 'xec':
                        # Contextual Tokenization: 'xec' as a keyword or identifier
                        if self.is_variable_declared(code):
                            token_type = 'IDENTIFIER'  # Treat 'xec' as an identifier in certain contexts
                        else:
                            token_type = 'KEYWORD'  # Treat 'xec' as a keyword

                    # Error Recovery for unclosed strings
                    if token_type == 'STRING' and not value.endswith('"'):
                        value += '"'  # Attempt to close the string
                        print(f"Warning: Unclosed string at line {self.line}, column {self.column}. Closing string automatically.")
                        token_type = 'STRING'  # Mark as a string even though it's incomplete

                    # Insert missing operators if necessary (basic error recovery)
                    if token_type == 'MISMATCH':
                        if self.last_token_is_identifier(code):
                            print(f"Warning: Missing operator at line {self.line}, column {self.column}. Inserting '+' operator.")
                            value = '+'
                            token_type = 'OPERATOR'

                    if token_type != 'WHITESPACE' and token_type != 'COMMENT':
                        tokens.append((token_type, value, self.line, self.column))
                    
                    self.position = match.end()
                    self.update_position(value)
                    break

            if not match:
                # If no match is found, perform error recovery
                self.handle_error(code)
                
        return tokens

    def update_position(self, value):
        # Update line and column based on the matched token
        lines = value.splitlines()
        if len(lines) > 1:
            self.line += len(lines) - 1
            self.column = len(lines[-1]) + 1
        else:
            self.column += len(value)

    def handle_error(self, code):
        """Error Recovery: Skip invalid characters."""
        # Output a helpful hint and skip the invalid character
        print(f"Warning: Skipping invalid character at line {self.line}, column {self.column}: '{code[self.position]}'")
        self.position += 1  # Skip the invalid character

    def is_variable_declared(self, code):
        """Check if 'xec' is used as a variable in the current context."""
        # Simple logic to check if 'xec' is followed by an identifier and assignment (e.g., 'xec myVar = 5')
        remaining_code = code[self.position:]
        pattern = r'\b[xec]\b\s+[a-zA-Z_][a-zA-Z0-9_]*\s*='  # Looking for 'xec myVar ='
        return bool(re.match(pattern, remaining_code))

    def last_token_is_identifier(self, code):
        """Check if the last token was an identifier (used for basic error recovery)."""
        # Look backwards at the previous token (for simplicity, just check the last token)
        remaining_code = code[:self.position]
        pattern = r'[a-zA-Z_][a-zA0-9_]*\s*$'
        return bool(re.search(pattern, remaining_code))

    def start_function_scope(self, function_name):
        """Start a new function scope to track local variables."""
        self.context[function_name] = {}
        self.current_scope = function_name
        print(f"Entering function scope: {function_name}")

    def end_function_scope(self):
        """End the current function scope."""
        print(f"Exiting function scope: {self.current_scope}")
        self.current_scope = 'global'

    def add_variable_to_scope(self, variable_name):
        """Add a variable to the current scope (local or global)."""
        if self.current_scope == 'global':
            self.context['global'][variable_name] = True
        else:
            self.context[self.current_scope][variable_name] = True
        print(f"Added variable '{variable_name}' to scope '{self.current_scope}'.")

# Sample usage
if __name__ == "__main__":
    sample_code = """
    function helloWorld() {
        xec myVar = 10;
        if (myVar > 5) {
            print("Greater than 5");
        } else {
            print("Less than or equal to 5");
        }
        return;
    }

    class MyClass {
        int memberVar;
        function myFunction() {
            return;
        }
    }

    xec arr = [1, 2, 3, 4];
    """

    lexer = Lexer()
    try:
        # Tokenize the sample code
        tokens = lexer.tokenize(sample_code)

        # Print tokens with their line and column information
        for token in tokens:
            print(f"({token[0]}, '{token[1]}', Line: {token[2]}, Column: {token[3]})")
    except SyntaxError as e:
        print(str(e))
