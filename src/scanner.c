#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct
{
  const char *start;
  const char *current;
  int line;
} Scanner;

Scanner scanner;

void
init_scanner (const char *source)
{
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

static bool
is_alpha (char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool
is_digit (char c)
{
  return c >= '0' && c <= '9';
}

static bool
is_at_end ()
{
  return *scanner.current == '\0';
}

static bool
is_end_of_line ()
{
  return *scanner.current == '\n';
}

static bool
is_whitespace (char c)
{
  if ((c == ' ') || (c == '\r') || (c == '\t') || (c == '\n'))
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
peek_next ()
{
  if (is_at_end ())
    return '\0';
  return scanner.current[1];
}

static bool
match (char expected)
{
  if (is_at_end ())
    return false;
  if (*scanner.current != expected)
    return false;
  scanner.current++;
  return true;
}

static Token
make_token (TokenType type)
{
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}

static Token
error_token (const char *message)
{
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen (message);
  token.line = scanner.line;
  return token;
}

static Token
skip_whitespace ()
{
  const char *start = scanner.current;
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
          return make_token (TOKEN_WHITESPACE);
        }
    }
}

/*  LaTeX comments go to the next \n char after the % char,
    scan the whole line, return as a single token

    NOTE: in order to figure out what the def is modifying, just take the next
    two tokens in the line.
    If the first token was a def or mathdef, you're saying you want to set-up
    a macro. What happens next is your fault.
    For tables, we're only concerned with the very next token, which should
    be a path.  */
static Token
comment ()
{
  char buf[255];
  int i = 0;
  bool first_token = true;
  TokenType type;
  while (peek () != '\n')
    {
      if (is_whitespace (peek ()) && first_token)
        {
          buf[i] = '\0'; /* End the string  */
          if (strcmp (buf, "def") == 0)
            {
              type = TOKEN_DEF;
              goto free;
            }
          if (strcmp (buf, "mathdef") == 0)
            {
              type = TOKEN_MATHDEF;
              goto free;
            }
          if (strcmp (buf, "table") == 0)
            {
              type = TOKEN_TABLE;
              goto free;
            }
          first_token = false;
        }
      if (first_token)
        {
          buf[i] = peek ();
          i++;
        }
      advance ();
    }
  type = TOKEN_COMMENT;
free:
  for (int i = 0; buf[i] != '\0'; i++)
    buf[i] = '\0';
  return make_token (type);
}

static Token
command ()
{
  char pre[255];
  char cmd[255];
  bool in_pre = true;
  int i = 0;
  TokenType type = TOKEN_COMMAND;

  /*  Handle inline backslash-marked math mode.  */
  if (peek () == '[')
    return make_token (TOKEN_MATH_BEGIN);
  if (peek () == ']')
    return make_token (TOKEN_MATH_END);

  /*  Handle math mode with begin/end.  */
  while (!is_whitespace (peek ()) && !is_at_end ())
    {
      if ((in_pre) && (peek () != '{'))
        {
          pre[i] = peek ();
          i++;
        }
      else if (peek () == '{')
        {
          in_pre = false;
          pre[i] = '\0';
          i = 0;
        }
      else if ((!in_pre) && (peek () != '}'))
        {
          cmd[i] = peek ();
          i++;
        }
      advance ();
    }
  cmd[i] = '\0';
  if ((strcmp (cmd, "equation") == 0) || (strcmp (cmd, "equation*") == 0)
      || (strcmp (cmd, "align") == 0) || (strcmp (cmd, "align*") == 0))
    {
      if (strcmp (pre, "begin") == 0)
        {
          type = TOKEN_MATH_BEGIN;
          goto free;
        }
      else if (strcmp (pre, "end") == 0)
        {
          type = TOKEN_MATH_END;
          goto free;
        }
    }

free:
  for (i = 0; pre[i] != '\0'; i++)
    pre[i] = '\0';
  for (i = 0; cmd[i] != '\0'; i++)
    cmd[i] = '\0';
  return make_token (type);
}

static Token
word ()
{
  while (is_alpha (peek ()) || is_digit (peek ()))
    advance ();
  return make_token (TOKEN_WORD);
}

static Token
delimited_word (char end)
{
  while (!match (end))
    if (!is_end_of_line ())
      advance ();
    else
      /*  Make sure we close the delimiter before the end of the line  */
      return make_token (TOKEN_ERROR);
  return make_token (TOKEN_WORD);
}

static Token
wildcard ()
{
  char pn = peek ();
  if (pn == 'w')
    {
      advance ();
      return make_token (TOKEN_WILDCARDWORD);
    }
  else if (pn == 's')
    {
      advance ();
      return make_token (TOKEN_WILDCARDSTRING);
    }
  else if (pn == 'i')
    {
      advance ();
      return make_token (TOKEN_WILDCARDNUMBER);
    }
  else if (is_digit (pn))
    {
      while (is_digit (peek ()))
        advance ();
      return make_token (TOKEN_WILDCARDBODY);
    }
  return word ();
  
}

/*  Scan for both integers and floating point numbers.  */
static Token
number ()
{
  while (is_digit (peek ()))
    advance ();
  if (match ('.'))
    advance ();
  else
    return make_token (TOKEN_WORDNUMBER);
  while (is_digit (peek ()))
    advance ();
  return make_token (TOKEN_WORDNUMBER);
  
}

Token
scan_token ()
{
  scanner.start = scanner.current;

  if (is_at_end ())
    return make_token (TOKEN_EOF);

  /*  What we want to do is use spaces as a delimiter and figure out what
      each token looks like based on that.
      There are no numbers or letters or symbols, it's all semantic  */

  char c = advance ();
  switch (c)
    {
    case ' ':
    case '\r':
    case '\t':
      return skip_whitespace ();
    case '\n':
      scanner.line++;
      return skip_whitespace ();
    case '%':
      return comment ();
    case '\\':
      return command ();
    case '#':
      return wildcard ();      
    case '{':
      return make_token (TOKEN_OPENCURL);
    case '}':
      return make_token (TOKEN_CLOSECURL);
    case '(':
      return make_token (TOKEN_OPENPAREN);
    case ')':
      return make_token (TOKEN_CLOSEPAREN);
    case '[':
      return make_token (TOKEN_OPENSQUARE);
    case ']':
      return make_token (TOKEN_CLOSESQUARE);
    case '"':
      return make_token (TOKEN_DOUBLEQUOTE);
    case '\'':
      return make_token (TOKEN_SINGLEQUOTE);
    case '$':
      return make_token (TOKEN_MATHDOLLAR_BEGIN);
    default:
      {
        if (is_digit (c))
          return number ();
        else if (is_alpha (c))
          return word ();
        else
          return make_token (TOKEN_ETCSYMBOL);
      }
    }
  /*  It should never execute this code  */
  return error_token ("Unexpected character.");
}
