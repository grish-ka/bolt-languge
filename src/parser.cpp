#include "parser.hpp"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

ProgramNode Parser::parse() {
    ProgramNode program;
    while (!is_at_end()) {
        try {
            program.statements.push_back(parse_declaration());
        } catch (const std::exception& e) {
            std::cerr << "Parse Error: " << e.what() << std::endl;
            // For now, we'll stop at the first error.
            break;
        }
    }
    return program;
}

// --- Helper Functions ---

bool Parser::is_at_end() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::advance() {
    if (!is_at_end()) m_current_pos++;
    return m_tokens[m_current_pos - 1];
}

Token Parser::peek() {
    return m_tokens[m_current_pos];
}

bool Parser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().type == type;
}

Token Parser::expect(TokenType type, const std::string& error_message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(error_message);
}

// --- Grammar Parsing Functions ---

std::unique_ptr<StmtNode> Parser::parse_declaration() {
    // A C-like program is a list of declarations.
    // For now, we only know one kind: function definitions.
    
    // Look for: int main ...
    if (check(TokenType::INT) && m_tokens[m_current_pos + 1].type == TokenType::IDENTIFIER) {
        return parse_function_definition();
    }
    
    // We'll skip over other things for now
    std::cerr << "Parser Warning: Skipping unknown top-level token: " << advance().to_string() << std::endl;
    return nullptr;
}

std::unique_ptr<StmtNode> Parser::parse_function_definition() {
    // 1. Consume the return type (e.g., "int")
    Token type = advance();
    
    // 2. Consume the name (e.g., "main")
    Token name = expect(TokenType::IDENTIFIER, "Expected function name.");

    // 3. Consume the open parenthesis
    expect(TokenType::OPEN_PAREN, "Expected '(' after function name.");
    
    // TODO: Parse parameters here
    
    // 4. Consume the close parenthesis
    expect(TokenType::CLOSE_PAREN, "Expected ')' after parameters.");
    
    // 5. Parse the function body (a block statement)
    std::unique_ptr<BlockStmtNode> body = parse_block_statement();
    
    return std::make_unique<FunctionDefNode>(type.value, name.value, std::move(body));
}

std::unique_ptr<BlockStmtNode> Parser::parse_block_statement() {
    // Consume the '{'
    expect(TokenType::OPEN_BRACE, "Expected '{' to begin a block.");
    
    auto block = std::make_unique<BlockStmtNode>();
    
    // Keep parsing statements until we hit the '}'
    while (!check(TokenType::CLOSE_BRACE) && !is_at_end()) {
        auto stmt = parse_statement();
        if (stmt) {
             block->statements.push_back(std::move(stmt));
        }
    }
    
    // Consume the '}'
    expect(TokenType::CLOSE_BRACE, "Expected '}' to end a block.");
    
    return block;
}
/*
stdSuch as:
* **Variable Declarations:** `int x = 10;`
* **Return Statements:** `return 0;`
*/
std::unique_ptr<StmtNode> Parser::parse_statement() {
    // This is where we decide what *kind* of statement we're looking at.
    if (check(TokenType::RETURN)) {
        return parse_return_statement();
    }
    
    // If it's not a known statement, we'll just advance and ignore it for now.
    // This is a very simple (and bad) error recovery.
    std::cerr << "Parser Warning: Skipping unknown token in block: " << advance().to_string() << std::endl;
    return nullptr; // Skipped token
}

std::unique_ptr<StmtNode> Parser::parse_return_statement() {
    // Consume the 'return' token
    advance(); 

    // Parse the expression that follows
    std::unique_ptr<ExprNode> expr = parse_expression();

    // Consume the semicolon
    expect(TokenType::SEMICOLON, "Expected ';' after return value.");

    return std::make_unique<ReturnStmtNode>(std::move(expr));
}

std::unique_ptr<ExprNode> Parser::parse_expression() {
    // This is a *very* simple expression parser.
    // It only handles number literals for now.
    if (check(TokenType::NUMBER_LITERAL)) {
        Token num = advance();
        return std::make_unique<NumberLiteralNode>(num.value);
    }

    // Default case
    throw std::runtime_error("Expected an expression (e.g., a number).");
}