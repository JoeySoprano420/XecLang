#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

// Enum for ASTNode types (simplified for the example)
enum class ASTNodeType {
    FUNCTION_DECLARATION,
    FUNCTION_CALL,
    ASSIGNMENT,
    VARIABLE_DECLARATION,
    BINARY_OPERATION,
    LITERAL,
    IDENTIFIER,
    IF_STATEMENT,
    RETURN_STATEMENT,
    BLOCK,
    ERROR
};

// Class representing an Abstract Syntax Tree (AST) node
class ASTNode {
public:
    ASTNodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(ASTNodeType type, std::string value = "") : type(type), value(value) {}

    void addChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
};

// Symbol table entry for variables, functions, etc.
struct Symbol {
    std::string name;
    std::string type;
    bool isFunction;
};

// Class for semantic analysis with symbol table management and detailed checks
class SemanticAnalyzer {
public:
    SemanticAnalyzer(std::shared_ptr<ASTNode> root) : root(root) {}

    void analyze() {
        analyzeNode(root);
        checkForUndeclaredVariables();
        checkFunctionOverloading();
    }

private:
    std::shared_ptr<ASTNode> root;
    std::unordered_map<std::string, Symbol> symbolTable;  // Stores symbols for variables and functions
    std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>> functionCalls; // Function call references
    std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>> variableReferences; // Variable usage references
    std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>> blocks; // Blocks for scope tracking

    void analyzeNode(std::shared_ptr<ASTNode> node) {
        switch (node->type) {
            case ASTNodeType::FUNCTION_DECLARATION:
                checkFunctionDeclaration(node);
                break;
            case ASTNodeType::ASSIGNMENT:
                checkAssignment(node);
                break;
            case ASTNodeType::VARIABLE_DECLARATION:
                checkVariableDeclaration(node);
                break;
            case ASTNodeType::FUNCTION_CALL:
                checkFunctionCall(node);
                break;
            case ASTNodeType::BINARY_OPERATION:
                checkBinaryOperation(node);
                break;
            case ASTNodeType::RETURN_STATEMENT:
                checkReturnStatement(node);
                break;
            case ASTNodeType::IF_STATEMENT:
                checkIfStatement(node);
                break;
            default:
                break;
        }

        for (auto &child : node->children) {
            analyzeNode(child);
        }
    }

    void checkFunctionDeclaration(std::shared_ptr<ASTNode> node) {
        // Function name should be unique in the symbol table
        if (symbolTable.find(node->value) != symbolTable.end()) {
            throw std::runtime_error("Function already declared: " + node->value);
        }

        // Register function in symbol table
        Symbol funcSymbol{node->value, "function", true};
        symbolTable[node->value] = funcSymbol;

        // Check parameters for valid types
        for (auto &child : node->children) {
            if (child->type == ASTNodeType::VARIABLE_DECLARATION) {
                checkVariableDeclaration(child);
            }
        }
    }

    void checkVariableDeclaration(std::shared_ptr<ASTNode> node) {
        // Check for redeclaration of variables
        if (symbolTable.find(node->value) != symbolTable.end()) {
            throw std::runtime_error("Variable already declared: " + node->value);
        }

        // Register variable in symbol table
        Symbol varSymbol{node->value, "variable", false};
        symbolTable[node->value] = varSymbol;
    }

    void checkAssignment(std::shared_ptr<ASTNode> node) {
        // Ensure the variable is declared
        std::string varName = node->value; // The variable name is the value of the assignment node
        if (symbolTable.find(varName) == symbolTable.end()) {
            throw std::runtime_error("Undeclared variable: " + varName);
        }

        // Type check the right-hand side (RHS) expression
        auto rhsNode = node->children[0];
        if (rhsNode->type == ASTNodeType::VARIABLE_DECLARATION || rhsNode->type == ASTNodeType::FUNCTION_CALL) {
            // Handle type compatibility (simplified for this example)
            checkAssignmentType(rhsNode);
        }
    }

    void checkAssignmentType(std::shared_ptr<ASTNode> node) {
        // Ensure types are compatible for assignment
        if (node->type == ASTNodeType::FUNCTION_CALL) {
            // Handle function call return type checking
        }
        // Handle other types of assignments (variables, literals)
    }

    void checkFunctionCall(std::shared_ptr<ASTNode> node) {
        // Check if function is declared
        if (symbolTable.find(node->value) == symbolTable.end() || !symbolTable[node->value].isFunction) {
            throw std::runtime_error("Undeclared function: " + node->value);
        }

        // Record the function call for potential later analysis (overloading, etc.)
        functionCalls[node->value].push_back(node);
    }

    void checkBinaryOperation(std::shared_ptr<ASTNode> node) {
        // Ensure operands are of compatible types
        auto lhs = node->children[0];
        auto rhs = node->children[1];

        if (lhs->type != rhs->type) {
            throw std::runtime_error("Type mismatch in binary operation");
        }
    }

    void checkReturnStatement(std::shared_ptr<ASTNode> node) {
        // Check if return type matches function's declared type
        // Assuming function return type is stored somewhere (simplified)
        auto functionNode = findFunctionDeclaration(node);
        if (functionNode && functionNode->value != node->value) {
            throw std::runtime_error("Return type mismatch for function: " + node->value);
        }
    }

    void checkIfStatement(std::shared_ptr<ASTNode> node) {
        // Check if condition expression is boolean
        auto condition = node->children[0];
        if (condition->type != ASTNodeType::LITERAL || condition->value != "bool") {
            throw std::runtime_error("If condition must be of boolean type");
        }
    }

    std::shared_ptr<ASTNode> findFunctionDeclaration(std::shared_ptr<ASTNode> node) {
        // Placeholder for finding the corresponding function declaration
        return nullptr;
    }

    void checkForUndeclaredVariables() {
        // After analysis, check if any variables are used without declaration
        for (auto &ref : variableReferences) {
            if (symbolTable.find(ref.first) == symbolTable.end()) {
                throw std::runtime_error("Undeclared variable used: " + ref.first);
            }
        }
    }

    void checkFunctionOverloading() {
        // Check for functions with the same name but different signatures (overloading)
        for (auto &entry : functionCalls) {
            if (entry.second.size() > 1) {
                // Placeholder for actual overloading detection logic
                std::cout << "Function " << entry.first << " is overloaded!" << std::endl;
            }
        }
    }

    void handleErrors(std::shared_ptr<ASTNode> node) {
        // Collect errors from the analysis phase
        if (node->type == ASTNodeType::ERROR) {
            std::cerr << "Error in node with value: " << node->value << std::endl;
        }
    }
};

int main() {
    // Sample code for testing semantic analysis
    auto root = std::make_shared<ASTNode>(ASTNodeType::FUNCTION_DECLARATION, "main");
    root->addChild(std::make_shared<ASTNode>(ASTNodeType::VARIABLE_DECLARATION, "x"));
    root->addChild(std::make_shared<ASTNode>(ASTNodeType::ASSIGNMENT, "x"));

    SemanticAnalyzer analyzer(root);
    try {
        analyzer.analyze();
        std::cout << "Semantic analysis passed." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Semantic analysis failed: " << e.what() << std::endl;
    }

    return 0;
}
