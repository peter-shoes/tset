#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    // single-char tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA, TOKEN_PLUS, TOKEN_STAR,
    TOKEN_LINE, TOKEN_TILDE, TOKEN_AND,
    TOKEN_GREATER, TOKEN_LESS,
    
    
    // confused multi-character tokens
    TOKEN_EQUAL, TOKEN_ASSIGN,
    TOKEN_MINUS, TOKEN_GUARD,
    TOKEN_SLASH, TOKEN_SLASH_TILDE,

    TOKEN_BANG, TOKEN_BANG_EQUAL, 
    TOKEN_BANG_LINE, TOKEN_BANG_TILDE, 
    TOKEN_BANG_AND, TOKEN_BANG_SLASH_TILDE,
    // literals
    TOKEN_IDENTIFIER, TOKEN_NUMBER,
    // keywords
    TOKEN_PROGRAM, TOKEN_END, 
    TOKEN_FOR, TOKEN_WHILE,
    TOKEN_V, TOKEN_Z, TOKEN_R,

    // special
    TOKEN_ERROR, TOKEN_EOF
}TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

void initScanner(const char* source);
Token scanToken();

#endif