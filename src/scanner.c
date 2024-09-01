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

/*  LaTeX comments go to the next \n char after the % char, 
    scan the whole line, return as a single token
    
    NOTE: in order to figure out what the def is modifying, just take the next
    two tokens in the line.
    If the first token was a def or mathdef, you're saying you want to set-up
    a macro. What happens next is your fault.
    Conversely, if there are more than three tokens in a def line, the rest 
    will be disregarded as comments.  
    For tables, we're only concerned with the very next token, which should
    be a path. Again, anything after that is considered a comment  */
static Token
comment ()
{
  char buf[255];
  int i = 0;
  bool first_token = true;
  while (peek () != '\n')
    {
      if (isWhitespace (peek ()) && first_token)
        {
          buf[i] = '\0'; /* End the string  */
          if (strcmp (buf, "def") == 0)
            return makeToken (TOKEN_DEF);
          if (strcmp (buf, "mathdef") == 0)
            return makeToken (TOKEN_MATHDEF);
          if (strcmp (buf, "table") == 0)
            return makeToken (TOKEN_TABLE);
          first_token = false;
        }
      if (first_token)
        {
          buf[i] = peek ();
          i++;
        }
      advance ();
    }
  return makeToken (TOKEN_COMMENT);
}
static Token
command ()
{
  while (!isWhitespace ( peek ()) && !isAtEnd ())
    advance();
  return makeToken (TOKEN_COMMAND);
}

static Token
word ()
{
  while (!isWhitespace ( peek ()) && !isAtEnd ())
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
  switch (c)
    {
      case '%':
        return comment ();
      case '\\':
        return command ();
      default:
        return word ();
    }
  /*  It should never execute this code  */
  return errorToken("Unexpected character.");
}
