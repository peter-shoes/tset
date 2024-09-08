#ifndef SCANNER_H
#define SCANNER_H

typedef enum 
{
  /*  LaTeX Keywords  */
  TOKEN_COMMAND,
  TOKEN_MATH_BEGIN, TOKEN_MATH_END,
  TOKEN_COMMENT,
  /*  T Keywords  */
  TOKEN_DEF, TOKEN_MATHDEF,
  TOKEN_OLDDEF, TOKEN_NEWDEF,
  TOKEN_TABLE,
  TOKEN_WORD,

  // special
  TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct
{
  TokenType type;
  const char* start;
  int length;
  int line;
} Token;

void init_scanner (const char* source);
Token scan_token ();

#endif
