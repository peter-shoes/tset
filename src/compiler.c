#include <stdio.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "stack.h"

#define PAREN 0x2
#define SQUARE 0x4
#define CURL 0x8

static int line = -1;

static void
compiler_fini (Token token)
{
  /*  Allocate a buffer and push onto the stack  */
  Token *buf = (Token *)malloc (sizeof (Token));
  *buf = token;

#ifdef DEBUG_SCANNER
  debug_token (token, &line);
#endif
  if (push_token (buf) == 1)
    {
      fprintf (stderr, "Error: failed to push token on line %d\n", token.line);
      exit (1);
    }
}

static void
stack_fini (Token pop, FILE *fptr)
{
#ifdef DEBUG_STACK
  debug_token (pop, &line);
#endif
  /*  Write to file  */
#ifndef DEBUG_STACK
  fprintf (fptr, "%.*s", pop.length, pop.start);
#endif
}
/*  Because LaTex is a semantic language, context is important.
    For that reason, sometimes we must alter the type of the token based on
    that context.  */
void
compile (const char *source, const char *outpath)
{
  init_scanner (source);
  init_stack ();
  int cur_bracket = 0; /*  Tracks if we are in a bracket, and which.  */
  bool in_mathdollar = false;
  bool in_mathmode = false;

#ifdef DEBUG_SCANNER
  printf ("\n======== SCANNER DEBUG TRACE ========\n");
#endif

  for (;;)
    {
      Token token = scan_token ();

      switch (token.type)
        {
        
        /*  Set the bracket */
        case TOKEN_OPENPAREN:
          cur_bracket |= PAREN;
        case TOKEN_OPENSQUARE:
          cur_bracket |= SQUARE;
        case TOKEN_OPENCURL:
          cur_bracket |= CURL;

        /*  Check that the bracket was properly unset */
        case TOKEN_CLOSEPAREN:
          cur_bracket ^= PAREN;
          if (cur_bracket & PAREN)
            token.type = TOKEN_ERROR;
          break;
        case TOKEN_CLOSESQUARE:
          cur_bracket ^= SQUARE;
          if (cur_bracket & SQUARE)
            token.type = TOKEN_ERROR;
          break;
        case TOKEN_CLOSECURL:
          cur_bracket ^= CURL;
          if (cur_bracket & CURL)
            token.type = TOKEN_ERROR;
          break;
        
        case TOKEN_DEF:
        case TOKEN_MATHDEF:
          int defline = token.line;
          int nspaces = 0;
          int loc_cur_bracket = 0;
          compiler_fini (token);
          while (token.line == defline)
            {
              token = scan_token ();
              if (token.type == TOKEN_WHITESPACE)
                {
                  if (loc_cur_bracket == 0)
                    nspaces++;
                }
              else if (token.type == TOKEN_OPENPAREN)
                loc_cur_bracket |= PAREN;
              else if (token.type == TOKEN_CLOSEPAREN)
                loc_cur_bracket ^= PAREN;
              else if (token.type == TOKEN_OPENSQUARE)
                loc_cur_bracket |= SQUARE;
              else if (token.type == TOKEN_CLOSESQUARE)
                loc_cur_bracket ^= SQUARE;
              else if (token.type == TOKEN_OPENCURL)
                loc_cur_bracket |= CURL;
              else if (token.type == TOKEN_CLOSECURL)
                loc_cur_bracket ^= CURL;

              if (token.type == TOKEN_WILDCARDWORD
                       || token.type == TOKEN_WILDCARDSTRING
                       || token.type == TOKEN_WILDCARDBODY
                       || token.type == TOKEN_WHITESPACE);
              else if (nspaces == 1)
                token.type = TOKEN_MACRO;
              else if (nspaces == 2)
                token.type = TOKEN_MACROBODY;

              compiler_fini (token);
            }

        make_word:
        case TOKEN_COMMAND:
        case TOKEN_WORD:
          {
            if (in_mathmode || in_mathdollar)
              token.type = TOKEN_MATHWORD;
            break;
          }

        case TOKEN_ERROR:
          {
            fprintf (stderr,
                     "Error: scanner found error token on line %d\nThis makes "
                     "all future tokens defunct.\n",
                     token.line);
#ifndef DEBUG_SCANNER
            exit (1);
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
      compiler_fini (token);
      if (token.type == TOKEN_EOF)
        {
          line = -1;
          break;
        }
    }

#ifdef DEBUG_STACK
  printf ("\n======== STACK DEBUG TRACE ========\n");
#endif

  FILE *fptr;
  if (outpath == NULL)
    fptr = stdout;
  else
    {
      fptr = fopen (outpath, "w");
      if (fptr == NULL)
        {
          printf ("Error opening file!\n");
          exit (1);
        }
    }
  
  /*  Set up trackers for macros  */
  macro_track_t *def_track = (macro_track_t *)malloc (sizeof (macro_track_t));
  macro_track_t *def_track_tail = def_track;
  def_track->next = NULL;

  macro_track_t *mathdef_track
      = (macro_track_t *)malloc (sizeof (macro_track_t));
  macro_track_t *mathdef_track_tail = mathdef_track;
  mathdef_track->next = NULL;

  while (!is_stack_empty ())
    {
      stack_top:
      Token pop = pop_token ();


      /*  Handle macros  */
      if ((pop.type == TOKEN_DEF) || (pop.type == TOKEN_MATHDEF))
        {
          macro_track_t *track_tail;
          if (pop.type == TOKEN_DEF)
            track_tail = def_track_tail;
          else
            track_tail = mathdef_track_tail;

          stack_fini(pop, fptr);
          macro_store_t *new_store, *tmp;
          if ((new_store = malloc (sizeof (macro_store_t))) == NULL)
            goto oom;
          tmp = new_store;
          
          int defline = pop.line;
          bool eom = false;
          while (!eom)
            {
              pop = pop_token ();
              /* TODO: this can cause problems if the line is malformed.  */
              if (pop.type != TOKEN_WHITESPACE)
                {
                  tmp->node = pop;

                  if ((tmp->next = malloc (sizeof (macro_store_t))) == NULL)
                    goto oom;
                  tmp = tmp->next;
                }
              else if (pop.line != defline)
                goto end_macro;
              
              stack_fini(pop, fptr);
            }
          end_macro:
          track_tail->store = new_store;
          if ((track_tail->next = malloc (sizeof (macro_track_t))) == NULL)
            goto oom;
          track_tail = track_tail->next;
          goto stack_top;
        }
      
      /*  Match macros  */
      else if (pop.type != TOKEN_WHITESPACE)
        {
          macro_track_t *tmp_track;
          if (pop.type == TOKEN_MATHWORD || pop.type == TOKEN_MATHDOLLAR_BEGIN
              || pop.type == TOKEN_MATH_BEGIN)
            tmp_track = mathdef_track;
          else
            tmp_track = def_track;
          
          if (tmp_track->next == NULL)
            goto fini;
          
          bool match_found = false;
          while (!match_found && tmp_track->next != NULL)
            {
              match_found = !unwind_macro(tmp_track->store);
              if (!match_found)
                tmp_track = tmp_track->next;
            }
          
          macro_store_t *tmp_store = tmp_track->store;
          if (match_found)
            {
              /*  Because the match was found at the NEXT stack loc.  */
              stack_fini (pop, fptr);
              while ((pop = pop_token ()).type == TOKEN_WHITESPACE)
                stack_fini (pop, fptr);

              while (tmp_store->node.type != TOKEN_MACROBODY)
                {
                  if (pop.type != TOKEN_WHITESPACE)
                    tmp_store = tmp_store->next;
                  pop = pop_token();
                }
              while (tmp_store->next != NULL)
                {
                  stack_fini(tmp_store->node, fptr);
                  tmp_store = tmp_store->next;
                }
                
            }
        }
      fini:
      stack_fini(pop, fptr);
    }
  
  if (fptr == stdout)
    fprintf (fptr, "\n");
  // free_stack ();
  return;
  
  oom:
    fprintf(stderr, "out of memory");
    exit(1);
}
