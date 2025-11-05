/*
 * Bolt Compiler (bolt-compiler)
 * Main C++ Source File
 *
 * This is the main driver for the compiler. It will:
 * 1. Read the source file.
 * 2. Call the Lexer to get tokens.
 * 3. Call the Parser to build an AST.
 * 4. Call the Code Generator to create assembly.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "lexer.hpp"  // Step 1
#include "parser.hpp" // Step 2
#include "codegen.hpp" // Step 3

// Helper function to read a file into a string
std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "❌ Error: Could not open file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// --- AST Pretty Printer ---
// (We'll keep this for debugging)

void print_ast(const std::unique_ptr<ExprNode>& node, std::string indent = "") {
    if (auto num_node = dynamic_cast<NumberLiteralNode*>(node.get())) {
        std::cout << indent << "NumberLiteral(" << num_node->value << ")" << std::endl;
    } else {
        std::cout << indent << "Unknown ExprNode" << std::endl;
    }
}

void print_ast(const std::unique_ptr<StmtNode>& node, std::string indent = "");

void print_ast(const BlockStmtNode* block, std::string indent) {
    std::cout << indent << "BlockStmt:" << std::endl;
    for (const auto& stmt : block->statements) {
        print_ast(stmt, indent + "  ");
    }
}

void print_ast(const std::unique_ptr<StmtNode>& node, std::string indent) {
    if (!node) {
        std::cout << indent << "NullStatement" << std::endl;
        return;
    }
    
    if (auto func_node = dynamic_cast<FunctionDefNode*>(node.get())) {
        std::cout << indent << "FunctionDef(" << func_node->return_type << " " << func_node->name << ")" << std::endl;
        print_ast(func_node->body.get(), indent + "  ");
    } 
    else if (auto return_node = dynamic_cast<ReturnStmtNode*>(node.get())) {
        std::cout << indent << "ReturnStmt:" << std::endl;
        print_ast(return_node->expression, indent + "  ");
    }
    else if (auto block_node = dynamic_cast<BlockStmtNode*>(node.get())) {
        print_ast(block_node, indent);
    }
    else {
        std::cout << indent << "Unknown StmtNode" << std::endl;
    }
}

// --- Main Compiler Driver ---

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: bolt-compiler <source-file>" << std::endl;
        return 1;
    }

    std::string source_file = argv[1];
    std::string output_file = "output.asm";
    std::cout << "Compiling " << source_file << "..." << std::endl;

    std::string source_code = read_file(source_file);
    if (source_code.empty()) {
        return 1;
    }

    // --- 1. LEXER STAGE ---
    std::cout << "--- [Lexer] ---" << std::endl;
    Lexer lexer(source_code);
    std::vector<Token> tokens = lexer.tokenize();
    // (We'll hide the verbose output for now)
    // for (const auto& token : tokens) {
    //     std::cout << token.to_string() << std::endl;
    // }

    // --- 2. PARSER STAGE ---
    std::cout << "--- [Parser] ---" << std::endl;
    Parser parser(tokens);
    ProgramNode ast = parser.parse();
    
    // (We'll hide the verbose output for now)
    // std::cout << "\n--- [Abstract Syntax Tree] ---" << std::endl;
    // for(const auto& stmt : ast.statements) {
    //     print_ast(stmt, "");
    // }

    // --- 3. CODEGEN STAGE ---
    std::cout << "\n--- [CodeGenerator] ---" << std::endl;
    CodeGenerator generator(std::move(ast)); // <-- CHANGED: Pass 'ast' by move
    std::string asm_code = generator.generate();
    
    std::cout << "Generated " << asm_code.length() << " bytes of assembly." << std::endl;

    // Write the assembly to output.asm
    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "❌ Error: Could not open output file: " << output_file << std::endl;
        return 1;
    }
    out << asm_code;
    out.close();

    std::cout << "\n✅ Build finished. Assembly written to " << output_file << std::endl;
    std::cout << "   Run 'nasm -f elf64 " << output_file << "' to assemble." << std::endl;
    return 0;
}