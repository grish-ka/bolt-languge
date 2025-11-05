#pragma once

#include <string>
#include <vector>

// Define all the possible token types in our language
enum class TokenType {
    // Keywords
    INT,
    CHAR,
    RETURN,
    FOR,

    // Identifiers
    IDENTIFIER,

    // Literals
    NUMBER_LITERAL,
    STRING_LITERAL,

    // Operators & Punctuation
    SEMICOLON,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_ANGLE,     // <
    CLOSE_ANGLE,    // >
    EQUALS,         // =
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /

    // Misc
    INCLUDE,        // #include
    END_OF_FILE
};

// Represents a single token
struct Token {
    TokenType type;
    std::string value; // The actual text (e.g., "main", "10", ";")
    int line;

    // Helper to convert token to a string for debugging
    std::string to_string() const;
};

// The Lexer class
class Lexer {
public:
    // Constructor
    Lexer(std::string source);

    // Main function to tokenize the entire source code
    std::vector<Token> tokenize();

private:
    std::string m_source;
    int m_current_pos = 0;
    int m_line = 1;

    // Helper functions
    bool is_at_end();
    char advance();
    char peek();
    Token make_token(TokenType type, std::string value);
    Token make_token(TokenType type); // For single-char tokens

    // Functions to handle different token types
    Token handle_identifier();
    Token handle_number();
    Token handle_string();
    void skip_whitespace_and_comments();
};