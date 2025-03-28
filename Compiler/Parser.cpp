#include <iostream>
#include <vector>
#include <memory>

enum class ASTNodeType {
    PROGRAM,
    FUNCTION_DECLARATION,
    ASSIGNMENT,
    IDENTIFIER,
    LITERAL,
    BINARY_EXPR,
    IF_STATEMENT,
    WHILE_LOOP,
    RETURN_STATEMENT,
    STRUCT_DECLARATION,
    ENUM_DECLARATION
};

struct ASTNode {
    ASTNodeType type;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::string value;

    ASTNode(ASTNodeType t) : type(t) {}
};

class Parser {
public:
    Parser(std::vector<Token> tokens) : tokens(tokens), position(0) {}

    std::shared_ptr<ASTNode> parse() {
        return parseProgram();
    }

private:
    std::vector<Token> tokens;
    int position;

    std::shared_ptr<ASTNode> parseProgram() {
        auto programNode = std::make_shared<ASTNode>(ASTNodeType::PROGRAM);
        while (position < tokens.size()) {
            programNode->children.push_back(parseStatement());
        }
        return programNode;
    }

    std::shared_ptr<ASTNode> parseStatement() {
        if (tokens[position].value == "function") {
            return parseFunctionDeclaration();
        }
        // Handle other statements (if, assignment, etc.)
    }

    std::shared_ptr<ASTNode> parseFunctionDeclaration() {
        // Parse function declaration
        auto node = std::make_shared<ASTNode>(ASTNodeType::FUNCTION_DECLARATION);
        node->value = tokens[position + 1].value;  // Function name
        return node;
    }
};
