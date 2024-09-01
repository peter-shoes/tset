#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

void
debug (Token token, int *line)
{
  if (token.line != *line)
    {
      printf ("%4d ", token.line);
      *line = token.line;
    } 
  else
    printf ("   | ");
  printf ("%2d '%.*s'\n", token.type, token.length, token.start);
}

/*  Because LaTex is a semantic language, context is important.
    For that reason, sometimes we must alter the type of the token based on
    that context.  */
void 
compile (const char* source)
{
  initScanner (source);
  int line = -1;
  int defcounter = -1;
  int defline = -1;
  for(;;) 
    {
      Token token = scanToken ();
      
      /*  Handle the case where we have more or less tokens in a def line than
          expected.
          If we have fewer tokens than needed for a def (AKA 1 or 0), then we
          will only add appropriate tokens, and the next line won't trigger a
          token reassignment because the token line will not match the defline.
          In the case that there are more tokens than are required for a def,
          we should just treat those as comments.  */
      if (token.line != defline && defcounter == 0)
        defcounter = -1;
      if (token.line == defline && defcounter >= 0)
        {
          if (defcounter == 2)
            {
              token.type = TOKEN_OLDDEF;
              defcounter--;
            }
          else if (defcounter == 1)
            {
              token.type = TOKEN_NEWDEF;
              defcounter--;
            }
          else if (defcounter == 0)
            token.type = TOKEN_COMMENT;
        }

      
      if ((token.type == TOKEN_DEF) || (token.type == TOKEN_MATHDEF))
        {
          defcounter = 2;
          defline = token.line;
        }

      #ifdef DEBUG
      debug (token, &line);
      #endif
      
      

      if (token.type == TOKEN_EOF) break;
    }
}
