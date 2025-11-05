#include "codegen.hpp"
#include <iostream>

CodeGenerator::CodeGenerator(ProgramNode ast) : m_ast(std::move(ast)) {}

std::string CodeGenerator::generate() {
    // --- Assembly Preamble ---
    // .global main tells the linker that 'main' is the entry point
    // .section .text contains all the executable code
    
    m_output << "global main\n";
    m_output << "section .text\n";

    // --- Visit Top-Level Statements ---
    // This will loop through our AST and find all function definitions
    for (const auto& stmt : m_ast.statements) {
        visit(stmt.get());
    }

    return m_output.str();
}

// --- Statement Visitors ---

// This is the main "router" for statements.
void CodeGenerator::visit(StmtNode* node) {
    if (!node) return; // Skip null statements
    
    // We use dynamic_cast to figure out what *kind* of statement this is
    // and call the correct, specific visitor.
    
    if (auto func_def = dynamic_cast<FunctionDefNode*>(node)) {
        visit(func_def);
    } else if (auto return_stmt = dynamic_cast<ReturnStmtNode*>(node)) {
        visit(return_stmt);
    } else if (auto block_stmt = dynamic_cast<BlockStmtNode*>(node)) {
        visit(block_stmt);
    } else {
        // We don't know how to compile this type of statement yet!
        // This is fine, we'll just ignore it (like our parser does).
    }
}

void CodeGenerator::visit(FunctionDefNode* node) {
    // 1. Emit the function label (e.g., "main:")
    m_output << node->name << ":\n";

    // 2. Emit the function "prologue"
    //    - push rbp: Save the old base pointer
    //    - mov rbp, rsp: Set our new stack frame
    m_output << "  push rbp\n";
    m_output << "  mov rbp, rsp\n";

    // 3. Visit all statements in the function's body
    visit(node->body.get());

    // 4. Emit the function "epilogue"
    //    (Note: The 'return' statement will actually do this)
    //    If a function doesn't return, we'd add it here.
}

void CodeGenerator::visit(BlockStmtNode* node) {
    // A block is just a list of statements. We visit them in order.
    for (const auto& stmt : node->statements) {
        visit(stmt.get());
    }
}

void CodeGenerator::visit(ReturnStmtNode* node) {
    // 1. Visit the expression.
    //    This will put the expression's result (e.g., "0")
    //    into the 'rax' register.
    visit(node->expression.get());
    
    // 2. Emit the function "epilogue"
    //    - mov rsp, rbp: Restore the old stack pointer
    //    - pop rbp: Restore the old base pointer
    m_output << "  mov rsp, rbp\n";
    m_output << "  pop rbp\n";
    
    // 3. Emit the 'ret' (return) instruction
    m_output << "  ret\n";
}


// --- Expression Visitors ---

// This is the main "router" for expressions.
void CodeGenerator::visit(ExprNode* node) {
    if (auto num_literal = dynamic_cast<NumberLiteralNode*>(node)) {
        visit(num_literal);
    } else {
        // We don't know how to compile this expression yet
        std::cerr << "CodeGen Warning: Unknown expression type!" << std::endl;
    }
}

void CodeGenerator::visit(NumberLiteralNode* node) {
    // This is the simplest code gen!
    // We move the number's value into the 'rax' register.
    // 'rax' is the standard register for return values in C-like languages.
    m_output << "  mov rax, " << node->value << "\n";
}