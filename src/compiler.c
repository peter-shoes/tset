#include <stdio.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
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

  FILE *fptr;
  fptr = fopen("test/test_output.tex","w");
  if (fptr == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  /*  Set up trackers for macros  */
  macro_track_t *def_track = (macro_track_t*) malloc (sizeof (macro_track_t));
  macro_track_t *def_track_tail = def_track;
  def_track->next = NULL;
  
  macro_track_t *mathdef_track = (macro_track_t*) malloc (sizeof (macro_track_t));
  macro_track_t *mathdef_track_tail = mathdef_track;
  mathdef_track->next = NULL;

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
          switch (defcounter)
            {
              case 4:
              case 2:
                {
                  defcounter--;
                  break;
                }
              case 3:
                {
                  token.type = TOKEN_MACRO;
                  defcounter--;
                  break;
                }
              case 1:
                {
                  token.type = TOKEN_MACROBODY;
                  defcounter--;
                  break;
                }
              case 0:
                {
                  if (token.type == TOKEN_WHITESPACE)
                    break;
                  token.type = TOKEN_ERROR;
                  fprintf(stderr, "Error: too many tokens for def: line %d\n",
                        token.line);
                  #ifndef DEBUG_SCANNER
                  exit(1);
                  #endif
                  break;
                }
            }
        }

      if ((token.type == TOKEN_DEF) || (token.type == TOKEN_MATHDEF))
        {
          defcounter = 4;
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

      /*  Handle macros  */
      if (pop.type == TOKEN_DEF)
        if ((peek_next (0).type == TOKEN_MACRO) &&
              (peek_next (1).type == TOKEN_MACROBODY))
          {
            def_track_tail->macro = peek_next (0);
            def_track_tail->body = peek_next (1);
            def_track_tail->next = (macro_track_t*) 
                  malloc (sizeof (macro_track_t));
            def_track_tail = def_track_tail->next;
          }
      
      if (pop.type == TOKEN_WORD)
        {
          char wordbuf[255];
          char macrobuf[255];
          sprintf(wordbuf, "%.*s", pop.length, pop.start);
          macro_track_t *tmp = def_track;
          while (tmp != def_track_tail)
            {
              sprintf(macrobuf, "%.*s", tmp->macro.length, tmp->macro.start);
              if (strcmp (wordbuf, macrobuf) == 0)
              {
                pop.length = tmp->body.length;
                pop.start = tmp->body.start;
              }
              tmp = tmp->next;
            }
        }
      /*  Write to file  */
      fprintf(fptr, "%.*s", pop.length, pop.start);

    }
  free_stack ();
}
