#include <iostream>
#include <vector>
#include <regex>
#include <map>
#include <cctype>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    SYMBOL,
    COMMENT,
    TYPE,
    STRUCT,
    ENUM,
    TUPLE,
    ARRAY,
    EOF_TOKEN,
    ERROR
};

struct Token {
    TokenType type;
    std::string value;
    int line, column;
};

class Lexer {
public:
    Lexer(const std::string &source) : source(source), position(0), line(1), column(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (position < source.size()) {
            char currentChar = source[position];
            if (isspace(currentChar)) {
                handleWhitespace(currentChar, tokens);
            } else if (isalpha(currentChar) || currentChar == '_') {
                tokens.push_back(handleIdentifier());
            } else if (isdigit(currentChar)) {
                tokens.push_back(handleNumber());
            } else if (currentChar == '"') {
                tokens.push_back(handleString());
            } else if (currentChar == '/' && source[position + 1] == '/') {
                tokens.push_back(handleComment());
            } else if (currentChar == '/' && source[position + 1] == '*') {
                tokens.push_back(handleMultiLineComment());
            } else if (isOperator(currentChar)) {
                tokens.push_back(handleOperator());
            } else if (isSymbol(currentChar)) {
                tokens.push_back(handleSymbol());
            } else {
                tokens.push_back(handleError());
            }
        }
        // EOF token
        tokens.push_back({TokenType::EOF_TOKEN, "", line, column});
        return tokens;
    }

private:
    std::string source;
    int position;
    int line, column;

    const std::map<std::string, TokenType> keywords = {
        {"if", TokenType::KEYWORD}, {"else", TokenType::KEYWORD}, {"while", TokenType::KEYWORD},
        {"return", TokenType::KEYWORD}, {"int", TokenType::KEYWORD}, {"float", TokenType::KEYWORD},
        {"bool", TokenType::KEYWORD}, {"char", TokenType::KEYWORD}, {"void", TokenType::KEYWORD},
        {"struct", TokenType::STRUCT}, {"enum", TokenType::ENUM}, {"tuple", TokenType::TUPLE}, {"array", TokenType::ARRAY}
    };

    void handleWhitespace(char &currentChar, std::vector<Token> &tokens) {
        if (currentChar == '\n') {
            line++;
            column = 1;
        }
        position++;
        column++;
    }

    Token handleIdentifier() {
        int start = position;
        while (isalnum(source[position]) || source[position] == '_') {
            position++;
            column++;
        }
        std::string value = source.substr(start, position - start);
        if (keywords.find(value) != keywords.end()) {
            return {TokenType::KEYWORD, value, line, column};
        }
        return {TokenType::IDENTIFIER, value, line, column};
    }

    Token handleNumber() {
        int start = position;
        bool isFloat = false;
        bool isHex = false;
        bool isBinary = false;

        if (source[position] == '0' && (source[position + 1] == 'x' || source[position + 1] == 'b')) {
            // Hexadecimal or binary number
            isHex = (source[position + 1] == 'x');
            isBinary = (source[position + 1] == 'b');
            position += 2;
            column += 2;
        }

        while (isdigit(source[position]) || source[position] == '.' || 
               (isHex && (isxdigit(source[position]))) || 
               (isBinary && (source[position] == '0' || source[position] == '1'))) {
            position++;
            column++;
        }

        std::string value = source.substr(start, position - start);
        return {TokenType::NUMBER, value, line, column};
    }

    Token handleString() {
        int start = position;
        position++; column++;  // Skip the opening quote
        while (source[position] != '"' && position < source.size()) {
            if (source[position] == '\\') {  // Handle escape sequences
                position++;
                column++;
            }
            position++;
            column++;
        }
        position++; column++;  // Skip the closing quote
        return {TokenType::STRING, source.substr(start, position - start), line, column};
    }

    Token handleComment() {
        int start = position;
        while (position < source.size() && source[position] != '\n') {
            position++;
            column++;
        }
        return {TokenType::COMMENT, source.substr(start, position - start), line, column};
    }

    Token handleMultiLineComment() {
        int start = position;
        position += 2;
        column += 2;  // Skip /*

        while (position < source.size() && !(source[position] == '*' && source[position + 1] == '/')) {
            position++;
            column++;
        }

        if (position < source.size()) {
            position += 2;  // Skip */
            column += 2;
        }
        return {TokenType::COMMENT, source.substr(start, position - start), line, column};
    }

    bool isOperator(char currentChar) {
        return currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/' ||
               currentChar == '=' || currentChar == '<' || currentChar == '>' || currentChar == '&' ||
               currentChar == '|' || currentChar == '!' || currentChar == '%' || currentChar == '^' ||
               currentChar == '?' || currentChar == ':';
    }

    Token handleOperator() {
        char currentChar = source[position];
        std::string op(1, currentChar);
        position++;
        column++;
        // Handle two-character operators (e.g., ==, <=, &&)
        if ((currentChar == '=' || currentChar == '<' || currentChar == '>' || currentChar == '&' || currentChar == '|') && 
            position < source.size() && (source[position] == '=' || source[position] == currentChar)) {
            op += source[position];
            position++;
            column++;
        }
        return {TokenType::OPERATOR, op, line, column};
    }

    bool isSymbol(char currentChar) {
        return currentChar == '{' || currentChar == '}' || currentChar == '(' || currentChar == ')' || 
               currentChar == ';' || currentChar == ',' || currentChar == '[' || currentChar == ']' ||
               currentChar == '.';
    }

    Token handleSymbol() {
        char currentChar = source[position];
        position++;
        column++;
        return {TokenType::SYMBOL, std::string(1, currentChar), line, column};
    }

    Token handleError() {
        char currentChar = source[position];
        position++;
        column++;
        return {TokenType::ERROR, std::string(1, currentChar), line, column};
    }
};

int main() {
    // Sample input source code
    std::string sourceCode = R"(int main() {
        struct Person {
            string name;
            int age;
        };

        Person p = { "John", 30 };
        if (p.age > 18) {
            return 1;
        }

        // This is a comment
        /* This is a multi-line comment */
    })";

    // Create Lexer instance and tokenize the input
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    // Output the tokenized result
    for (const auto& token : tokens) {
        std::string tokenType;
        switch (token.type) {
            case TokenType::KEYWORD: tokenType = "KEYWORD"; break;
            case TokenType::IDENTIFIER: tokenType = "IDENTIFIER"; break;
            case TokenType::NUMBER: tokenType = "NUMBER"; break;
            case TokenType::STRING: tokenType = "STRING"; break;
            case TokenType::OPERATOR: tokenType = "OPERATOR"; break;
            case TokenType::SYMBOL: tokenType = "SYMBOL"; break;
            case TokenType::COMMENT: tokenType = "COMMENT"; break;
            case TokenType::STRUCT: tokenType = "STRUCT"; break;
            case TokenType::ENUM: tokenType = "ENUM"; break;
            case TokenType::TUPLE: tokenType = "TUPLE"; break;
            case TokenType::ARRAY: tokenType = "ARRAY"; break;
            case TokenType::ERROR: tokenType = "ERROR"; break;
            case TokenType::EOF_TOKEN: tokenType = "EOF_TOKEN"; break;
        }
        std::cout << "Type: " << tokenType << ", Value: '" << token.value << "' at line " << token.line << ", column " << token.column << std::endl;
    }

    return 0;
}
