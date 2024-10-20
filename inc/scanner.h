#ifndef SCANNER_H
#define SCANNER_H
#define SUBMACRO_SLOTS 16

typedef enum
{
  /*  LaTeX Keywords  */
  TOKEN_COMMAND,
  TOKEN_MATH_BEGIN,
  TOKEN_MATH_END,
  TOKEN_MATHDOLLAR_BEGIN,
  TOKEN_MATHDOLLAR_END,
  TOKEN_COMMENT,
  /*  T Keywords  */
  TOKEN_DEF,
  TOKEN_MATHDEF,
  TOKEN_MACRO,
  TOKEN_MACROBODY,
  TOKEN_WILDCARDWORD,
  TOKEN_WILDCARDSTRING,
  TOKEN_WILDCARDBODY,
  TOKEN_TABLE,
  TOKEN_WORD,
  TOKEN_MATHWORD,
  /*  Symbols  */
  TOKEN_WHITESPACE,
  TOKEN_OPENCURL,
  TOKEN_CLOSECURL,
  TOKEN_OPENSQUARE,
  TOKEN_CLOSESQUARE,
  TOKEN_OPENPAREN,
  TOKEN_CLOSEPAREN,
  TOKEN_SINGLEQUOTE,
  TOKEN_DOUBLEQUOTE,

  // special
  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

typedef struct
{
  TokenType type;
  const char *start;
  int length;
  int line;
} Token;

void init_scanner (const char *source);
Token scan_token ();

#endif
