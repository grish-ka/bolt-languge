#pragma once

#include "lexer.hpp"
#include <vector>
#include <memory> // For std::unique_ptr

// --- Abstract Syntax Tree (AST) Nodes ---
// These are the building blocks of our program's structure.

// Base class for all expressions (e.g., 10, "hello", x, a + b)
struct ExprNode {
    virtual ~ExprNode() = default;
};

// Base class for all statements (e.g., return, for, int x = ...)
struct StmtNode {
    virtual ~StmtNode() = default;
};

// Represents the entire program: a list of top-level statements/declarations
struct ProgramNode {
    std::vector<std::unique_ptr<StmtNode>> statements;
};

// --- Expression Nodes ---

// Represents a number literal, e.g., 10
struct NumberLiteralNode : public ExprNode {
    std::string value;
    NumberLiteralNode(std::string val) : value(std::move(val)) {}
};

// --- Statement Nodes ---

// Represents a block of statements: { ... }
struct BlockStmtNode : public StmtNode {
    std::vector<std::unique_ptr<StmtNode>> statements;
};

// Represents a 'return' statement, e.g., return 0;
struct ReturnStmtNode : public StmtNode {
    std::unique_ptr<ExprNode> expression;
    ReturnStmtNode(std::unique_ptr<ExprNode> expr) : expression(std::move(expr)) {}
};

// Represents: int main() { ... }
struct FunctionDefNode : public StmtNode {
    std::string return_type; // "int"
    std::string name;        // "main"
    // We will add parameters later
    std::unique_ptr<BlockStmtNode> body;

    FunctionDefNode(std::string ret_type, std::string n, std::unique_ptr<BlockStmtNode> b)
        : return_type(std::move(ret_type)), name(std::move(n)), body(std::move(b)) {}
};


// --- The Parser Class ---

class Parser {
public:
    Parser(std::vector<Token> tokens);

    // The main function that builds the AST
    ProgramNode parse();

private:
    std::vector<Token> m_tokens;
    int m_current_pos = 0;

    // Helper functions
    bool is_at_end();
    Token advance();
    Token peek();
    bool check(TokenType type);
    Token expect(TokenType type, const std::string& error_message);

    // Functions to parse different parts of the grammar
    std::unique_ptr<StmtNode> parse_declaration();
    std::unique_ptr<StmtNode> parse_statement();

    std::unique_ptr<StmtNode> parse_function_definition();
    std::unique_ptr<BlockStmtNode> parse_block_statement();
    std::unique_ptr<StmtNode> parse_return_statement();
    
    std::unique_ptr<ExprNode> parse_expression();
};