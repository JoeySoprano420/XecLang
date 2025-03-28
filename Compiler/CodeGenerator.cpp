class CodeGenerator {
public:
    CodeGenerator(std::shared_ptr<ASTNode> root) : root(root) {}

    void generate() {
        generateNode(root);
    }

private:
    std::shared_ptr<ASTNode> root;

    void generateNode(std::shared_ptr<ASTNode> node) {
        switch (node->type) {
            case ASTNodeType::FUNCTION_DECLARATION:
                generateFunctionDeclaration(node);
                break;
            case ASTNodeType::ASSIGNMENT:
                generateAssignment(node);
                break;
            default:
                break;
        }
    }

    void generateFunctionDeclaration(std::shared_ptr<ASTNode> node) {
        std::cout << "Generating function: " << node->value << std::endl;
    }

    void generateAssignment(std::shared_ptr<ASTNode> node) {
        std::cout << "Generating assignment" << std::endl;
    }
};
