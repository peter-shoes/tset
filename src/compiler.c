#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "stack.h"
#include "debug.h"

/*  Because LaTex is a semantic language, context is important.
    For that reason, sometimes we must alter the type of the token based on
    that context.  */
void 
compile (const char* source)
{
  init_scanner (source);
  init_stack ();
  int line = -1;
  int defcounter = -1;
  int defline = -1;

  #ifdef DEBUG_SCANNER
  printf("\n======== SCANNER DEBUG TRACE ========\n");
  #endif

  for(;;) 
    {
      Token token = scan_token ();
      
      /*  Handle the case where we have more or less tokens in a def line than
          expected.
          If we have fewer tokens than needed for a def (AKA 1 or 0), then we
          will only add appropriate tokens, and the next line won't trigger a
          token reassignment because the token line will not match the defline.
          In the case that there are more tokens than are required for a def,
          we consider those to be errors  */
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
            {
              token.type = TOKEN_ERROR;
              fprintf(stderr, "Error: too many tokens for def on line %d\n",
                    token.line);
              #ifndef DEBUG_SCANNER
              exit(1);
              #endif
            }

        }

      
      if ((token.type == TOKEN_DEF) || (token.type == TOKEN_MATHDEF))
        {
          defcounter = 2;
          defline = token.line;
        }

      #ifdef DEBUG_SCANNER
      debug_token (token, &line);
      #endif

      /*  Allocate a buffer and push onto the stack  */
      Token *buf = (Token*) malloc (sizeof(Token));
      *buf = token;
      if (push_token (buf) == 1)
        {
          fprintf(stderr, "Error: failed to push token on line %d\n", 
                token.line);
          exit(1);
        }
      
      if (token.type == TOKEN_EOF)
      {
        line = -1;
        break;
      }
    }

  #ifdef DEBUG_STACK
  printf("\n======== STACK DEBUG TRACE ========\n");
  #endif
  while (!is_stack_empty ())
    {
      Token pop = pop_token ();
      
      #ifdef DEBUG_STACK
      debug_token (pop, &line);
      #endif
    }
  free_stack ();
}
