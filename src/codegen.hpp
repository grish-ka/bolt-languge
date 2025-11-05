#pragma once

#include "parser.hpp" // We need the AST definitions
#include <string>
#include <sstream>

// This class walks the AST (from parser.hpp) and generates assembly code.
class CodeGenerator {
public:
    // Takes the root of the AST
    CodeGenerator(ProgramNode ast);

    // Main function to generate the assembly string
    std::string generate();

private:
    ProgramNode m_ast;
    std::stringstream m_output; // We build the assembly string here

    // --- Visitor Functions ---
    // We need one "visit" function for every AST node type.

    // Statements
    void visit(StmtNode* node);
    void visit(FunctionDefNode* node);
    void visit(ReturnStmtNode* node);
    void visit(BlockStmtNode* node);
    // (We'll add VarDeclNode, ForLoopNode, etc. later)

    // Expressions
    void visit(ExprNode* node);
    void visit(NumberLiteralNode* node);
    // (We'll add VarUsageNode, BinaryOpNode, etc. later)
};