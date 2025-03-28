int main() {
    // Sample input source code
    std::string sourceCode = "function main() { var x = 10; }";

    // 1. Tokenize the source code
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parse the tokens into an AST
    Parser parser(tokens);
    std::shared_ptr<ASTNode> ast = parser.parse();

    // 3. Perform semantic analysis
    SemanticAnalyzer semanticAnalyzer(ast);
    semanticAnalyzer.analyze();

    // 4. Generate code
    CodeGenerator codeGenerator(ast);
    codeGenerator.generate();

    return 0;
}
