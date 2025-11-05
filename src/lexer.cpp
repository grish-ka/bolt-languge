#include "lexer.hpp"
#include <iostream>
#include <unordered_map>

// Map of keywords to their token types
std::unordered_map<std::string, TokenType> keywords = {
    {"int",    TokenType::INT},
    {"char",   TokenType::CHAR},
    {"return", TokenType::RETURN},
    {"for",    TokenType::FOR}
};

// --- Token::to_string() ---
std::string Token::to_string() const {
    // This is just for debug printing, we'll make it better later
    std::string type_str;
    switch (type) {
        case TokenType::INT:            type_str = "INT"; break;
        case TokenType::CHAR:           type_str = "CHAR"; break;
        case TokenType::RETURN:         type_str = "RETURN"; break;
        case TokenType::FOR:            type_str = "FOR"; break;
        case TokenType::IDENTIFIER:     type_str = "IDENTIFIER"; break;
        case TokenType::NUMBER_LITERAL: type_str = "NUMBER_LITERAL"; break;
        case TokenType::STRING_LITERAL: type_str = "STRING_LITERAL"; break;
        case TokenType::SEMICOLON:      type_str = "SEMICOLON"; break;
        case TokenType::OPEN_PAREN:     type_str = "OPEN_PAREN"; break;
        case TokenType::CLOSE_PAREN:    type_str = "CLOSE_PAREN"; break;
        case TokenType::OPEN_BRACE:     type_str = "OPEN_BRACE"; break;
        case TokenType::CLOSE_BRACE:    type_str = "CLOSE_BRACE"; break;
        case TokenType::OPEN_ANGLE:     type_str = "OPEN_ANGLE"; break;
        case TokenType::CLOSE_ANGLE:    type_str = "CLOSE_ANGLE"; break;
        case TokenType::EQUALS:         type_str = "EQUALS"; break;
        case TokenType::PLUS:           type_str = "PLUS"; break;
        case TokenType::MINUS:          type_str = "MINUS"; break;
        case TokenType::STAR:           type_str = "STAR"; break;
        case TokenType::SLASH:          type_str = "SLASH"; break;
        case TokenType::INCLUDE:        type_str = "INCLUDE"; break;
        case TokenType::END_OF_FILE:    type_str = "END_OF_FILE"; break;
        default:                        type_str = "UNKNOWN"; break;
    }
    return "Token [Type: " + type_str + ", Value: '" + value + "', Line: " + std::to_string(line) + "]";
}

// --- Lexer Implementation ---

Lexer::Lexer(std::string source) : m_source(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!is_at_end()) {
        // We skip all whitespace *before* trying to find the next token
        skip_whitespace_and_comments();

        if (is_at_end()) break;

        char c = advance();

        // Handle single-character tokens
        switch (c) {
            case ';': tokens.push_back(make_token(TokenType::SEMICOLON)); break;
            case '(': tokens.push_back(make_token(TokenType::OPEN_PAREN)); break;
            case ')': tokens.push_back(make_token(TokenType::CLOSE_PAREN)); break;
            case '{': tokens.push_back(make_token(TokenType::OPEN_BRACE)); break;
            case '}': tokens.push_back(make_token(TokenType::CLOSE_BRACE)); break;
            case '<': tokens.push_back(make_token(TokenType::OPEN_ANGLE)); break;
            case '>': tokens.push_back(make_token(TokenType::CLOSE_ANGLE)); break;
            case '=': tokens.push_back(make_token(TokenType::EQUALS)); break;
            case '+': tokens.push_back(make_token(TokenType::PLUS)); break;
            case '-': tokens.push_back(make_token(TokenType::MINUS)); break;
            case '*': tokens.push_back(make_token(TokenType::STAR)); break;
            // Note: We'll handle '/' in skip_whitespace for comments

            // Handle multi-character tokens
            case '#':
                if (peek() == 'i') { // Crude check for #include
                    m_current_pos += 6; // Skip "include"
                    tokens.push_back(make_token(TokenType::INCLUDE, "#include"));
                }
                break;

            case '"':
                tokens.push_back(handle_string());
                break;

            default:
                if (std::isdigit(c)) {
                    // Backtrack one so handle_number() starts on the digit
                    m_current_pos--;
                    tokens.push_back(handle_number());
                } else if (std::isalpha(c) || c == '_') {
                    // Backtrack one
                    m_current_pos--;
                    tokens.push_back(handle_identifier());
                } else {
                    std::cerr << "Lexer Error: Unknown character '" << c << "' on line " << m_line << std::endl;
                }
                break;
        }
    }

    tokens.push_back(make_token(TokenType::END_OF_FILE, ""));
    return tokens;
}

bool Lexer::is_at_end() {
    return m_current_pos >= m_source.length();
}

char Lexer::advance() {
    return m_source[m_current_pos++];
}

char Lexer::peek() {
    if (is_at_end()) return '\0';
    return m_source[m_current_pos];
}

Token Lexer::make_token(TokenType type, std::string value) {
    return Token{type, std::move(value), m_line};
}

Token Lexer::make_token(TokenType type) {
    return Token{type, std::string(1, m_source[m_current_pos - 1]), m_line};
}

void Lexer::skip_whitespace_and_comments() {
    while (!is_at_end()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                m_line++;
                advance();
                break;
            case '/':
                if (m_current_pos + 1 < m_source.length() && m_source[m_current_pos + 1] == '/') {
                    // It's a single-line comment
                    while (peek() != '\n' && !is_at_end()) {
                        advance();
                    }
                } else {
                    // It's just a regular slash, not whitespace
                    return;
                }
                break;
            default:
                return; // Not whitespace
        }
    }
}

Token Lexer::handle_identifier() {
    int start = m_current_pos;
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string value = m_source.substr(start, m_current_pos - start);

    // Check if it's a keyword
    if (keywords.count(value)) {
        return make_token(keywords[value], value);
    }

    return make_token(TokenType::IDENTIFIER, value);
}

Token Lexer::handle_number() {
    int start = m_current_pos;
    while (std::isdigit(peek())) {
        advance();
    }
    std::string value = m_source.substr(start, m_current_pos - start);
    return make_token(TokenType::NUMBER_LITERAL, value);
}

Token Lexer::handle_string() {
    int start = m_current_pos; // The position *after* the opening "
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') m_line++;
        advance();
    }

    if (is_at_end()) {
        std::cerr << "Lexer Error: Unterminated string on line " << m_line << std::endl;
        return make_token(TokenType::END_OF_FILE, "ERROR"); // Improvise
    }

    std::string value = m_source.substr(start, m_current_pos - start);
    advance(); // Consume the closing "
    return make_token(TokenType::STRING_LITERAL, value);
}