#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct
{
  const char* start;
  const char* current;
  int line;
} Scanner;

Scanner scanner;

void
initScanner (const char* source)
{
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

static bool
isAlpha (char c)
{
  return  (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool
isDigit (char c)
{
  return c >= '0' && c <= '9';
}

static bool
isAtEnd () 
{
  return *scanner.current == '\0';
}

static bool
isWhitespace (char c)
{
  if (
    (c == ' ') ||
    (c == '\r') ||
    (c == '\t') ||
    (c == '\n')
  )
    return true;
  return false;
}

static char
advance ()
{
  scanner.current++;
  return scanner.current[-1];
}

static char 
peek ()
{
  return *scanner.current;
}

static char
peekNext () 
{
  if (isAtEnd ()) return '\0';
  return scanner.current[1];
}

static bool
match (char expected)
{
  if (isAtEnd ())
    return false;
  if (*scanner.current != expected)
    return false;
  scanner.current++;
  return true;
}

static Token
makeToken (TokenType type)
{
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int) (scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}

static Token
errorToken (const char* message)
{
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int) strlen (message);
  token.line = scanner.line;
  return token;
}

static void
skipWhitespace () 
{
  for (;;)
    {
      char c = peek ();
      switch (c)
        {
          case ' ':
          case '\r':
          case '\t':
            advance ();
            break;
          case '\n':
            scanner.line++;
            advance ();
            break;
          default:
            return;
        }
    }
}

static Token
comment ()
{
  char buf[255];
  int i = 0;
  while (isAlpha (peek ()))
  {
    buf[i] = peek ();
    i++;
    advance ();
  }
  return makeToken (TOKEN_COMMENT);
}

static Token
word ()
{
  while (!isWhitespace ( peek()) && !isAtEnd ())
    advance();
  return makeToken (TOKEN_WORD);
}

Token
scanToken ()
{
  skipWhitespace ();
  scanner.start = scanner.current;

  if (isAtEnd ())
    return makeToken (TOKEN_EOF);
  
  /*  What we want to do is use spaces as a delimiter and figure out what 
      each token looks like based on that.
      There are no numbers or letters or symbols, it's all semantic  */

  char c = advance ();
  if (c == '%')
    return comment ();
  return word ();
  // if (isDigit (c))
  //   return number ();

  // switch (c)
  //   {
  //     // one char
  //     case '(':
  //       return makeToken(TOKEN_LEFT_PAREN);
  //     case ')':
  //       return makeToken(TOKEN_RIGHT_PAREN);
  //     case '[':
  //       return makeToken(TOKEN_LEFT_BRACKET);
  //     case ']':
  //       return makeToken(TOKEN_RIGHT_BRACKET);
  //     case ',':
  //       return makeToken(TOKEN_COMMA);
  //     case '+': 
  //       return makeToken(TOKEN_PLUS);
  //     case '*':
  //       return makeToken(TOKEN_STAR);
  //     case '|':
  //       return makeToken(TOKEN_LINE);
  //     case '~':
  //       return makeToken(TOKEN_TILDE);
  //     case '&':
  //       return makeToken(TOKEN_AND);
  //     case '>':
  //       return makeToken(TOKEN_GREATER);
  //     case '<':
  //       return makeToken(TOKEN_LESS);
  //     // multi-char
  //     case '=':
  //       return makeToken(match('>') ? TOKEN_ASSIGN : TOKEN_EQUAL);
  //     case '-':
  //       return makeToken(match('>') ? TOKEN_GUARD : TOKEN_MINUS);
  //     case '/':
  //       return makeToken(match('~') ? TOKEN_SLASH_TILDE : TOKEN_SLASH);
  //     // bang stuff (binary negations)
  //     case '!' :
  //       {
  //         if (match('='))
  //           return makeToken(TOKEN_BANG_EQUAL);
  //         else if (match('|'))
  //           return makeToken(TOKEN_BANG_LINE);
  //         else if (match('~'))
  //           return makeToken(TOKEN_BANG_TILDE);
  //         else if (match('&'))
  //           return makeToken(TOKEN_BANG_AND);
  //         // avoiding bang_slash_tilde for now (idek what it does)
  //         else return makeToken(TOKEN_BANG);
  //       }
  //   }

  return errorToken("Unexpected character.");
}
