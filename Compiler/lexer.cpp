#include <iostream>
#include <vector>
#include <regex>
#include <map>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    SYMBOL,
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
            } else if (currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/') {
                tokens.push_back(handleOperator());
            } else if (currentChar == '{' || currentChar == '}' || currentChar == ';') {
                tokens.push_back(handleSymbol());
            } else {
                tokens.push_back(handleError());
            }
        }
        return tokens;
    }

private:
    std::string source;
    int position;
    int line, column;

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
        return {TokenType::IDENTIFIER, source.substr(start, position - start), line, column};
    }

    Token handleNumber() {
        int start = position;
        while (isdigit(source[position])) {
            position++;
            column++;
        }
        return {TokenType::NUMBER, source.substr(start, position - start), line, column};
    }

    Token handleString() {
        int start = position;
        position++;
        column++;
        while (source[position] != '"' && position < source.size()) {
            position++;
            column++;
        }
        position++;
        column++;
        return {TokenType::STRING, source.substr(start, position - start), line, column};
    }

    Token handleOperator() {
        char currentChar = source[position];
        position++;
        column++;
        return {TokenType::OPERATOR, std::string(1, currentChar), line, column};
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
