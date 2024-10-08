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
compile (const char* source, const char *outpath)
{
  init_scanner (source);
  init_stack ();
  int line = -1;
  bool in_mathdollar = false;
  bool in_mathmode = false;

  FILE *fptr;
  if (outpath == NULL)
    fptr = stdout;
  else
    {
      fptr = fopen(outpath,"w");
      if (fptr == NULL) {
        printf("Error opening file!\n");
        exit(1);
      }
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

      switch (token.type)
        {
          case TOKEN_COMMAND:
          case TOKEN_WORD:
            {
              int token_type = peek_last ().type;
              if ((token_type == TOKEN_DEF) || (token_type == TOKEN_MATHDEF))
                token.type = TOKEN_MACRO;
              else if (token_type == TOKEN_MACRO)
                token.type = TOKEN_MACROBODY;
              else if (in_mathmode)
                token.type = TOKEN_MATHWORD;
              break;
            }
          
          case TOKEN_ERROR:
            {
              fprintf(stderr, "Error: scanner found error token on line %d\nThis makes all future tokens defunct.\n",
                    token.line);
              #ifndef DEBUG_SCANNER
              exit(1);
              #endif
              break;
            }
          
          case TOKEN_MATHDOLLAR_BEGIN:
            {
              if (in_mathdollar)
                {
                  token.type = TOKEN_MATHDOLLAR_END;
                  in_mathdollar = false;
                  in_mathmode = false;
                }
              else
              {
                in_mathdollar = true;
                in_mathmode = true;
              }
              break;
            }
          case TOKEN_MATH_BEGIN:
            {
              in_mathmode = true;
              break;
            }
          case TOKEN_MATH_END:
            {
              in_mathmode = false;
              break;
            }
          
          default:
            break;
        }
      
      /*  Allocate a buffer and push onto the stack  */
      Token *buf = (Token*) malloc (sizeof(Token));
      *buf = token;
      
      #ifdef DEBUG_SCANNER
      debug_token (token, &line);
      #endif
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
      if ((pop.type == TOKEN_DEF)||(pop.type == TOKEN_MATHDEF))
        if ((peek_next (0).type == TOKEN_MACRO) &&
              (peek_next (1).type == TOKEN_MACROBODY))
          {
            macro_track_t* tail = (pop.type == TOKEN_DEF) ? def_track_tail : mathdef_track_tail;
            tail->macro = peek_next (0);
            tail->body = peek_next (1);
            tail->next = (macro_track_t*) 
                  malloc (sizeof (macro_track_t));
            if (pop.type == TOKEN_DEF)
              def_track_tail = tail->next;
            else
              mathdef_track_tail = tail->next;
          }
      
      if ((pop.type == TOKEN_WORD)||(pop.type == TOKEN_MATHWORD))
        {
          char wordbuf[255];
          char macrobuf[255];
          sprintf(wordbuf, "%.*s", pop.length, pop.start);
          macro_track_t *tmp = (pop.type == TOKEN_WORD) ? def_track : mathdef_track;
          macro_track_t *tail = (pop.type == TOKEN_WORD) ? def_track_tail : mathdef_track_tail;
          while (tmp != tail)
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
  if (fptr == stdout)
    fprintf(fptr, "\n");
  free_stack ();
}
