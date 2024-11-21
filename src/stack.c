#include "stack.h"
#include "common.h"
#include <stdio.h>

stack_t *program_stack;
stack_t *stack_tail;
Token *last_non_whitespace;

int
init_stack ()
{
  if ((program_stack = (stack_t *)malloc (sizeof (stack_t))) == NULL)
    return 1;
  stack_tail = program_stack;
  program_stack->node = NULL;
  program_stack->next = NULL;
  return 0;
}

/*  If you're calling this, the program should be over, and therefore the only
    token left in the stack should be a TOKEN_EOF, so you only need to free
    a single node. This was designed intentionally.  */
void
free_stack ()
{
  free (program_stack);
}

int
push_token (Token *token)
{
  if (program_stack->node == NULL)
    {
      program_stack->node = token;
      return 0;
    }

  stack_t *new_node;
  if ((new_node = (stack_t *)malloc (sizeof (stack_t))) == NULL)
    return 1;
  new_node->node = token;
  new_node->next = NULL;
  stack_tail->next = new_node;
  stack_tail = stack_tail->next;
  if (token->type != TOKEN_WHITESPACE)
    last_non_whitespace = new_node->node;
  return 0;
}

/*  Note that you should call is_stack_empty() before calling this to
    avoid a segfault in the case that the stack is empty. */
Token
pop_token ()
{
  Token tmp;
  stack_t *hold;

  tmp = *program_stack->node;
  hold = program_stack;
  program_stack = program_stack->next;
  free (hold);
  return tmp;
}

int
is_stack_empty ()
{
  if (program_stack->node->type == TOKEN_EOF)
    return 1;
  return 0;
}

Token
peek_next (int depth)
{
  stack_t *tmp = program_stack;
  while (depth > 0)
    {
      if (tmp->next != NULL)
        {
          tmp = tmp->next;
          // if (tmp->node->type != TOKEN_WHITESPACE)
          depth--;
        }
      else
        {
          printf ("Error: requesting invalid depth\n");
          break;
        }
    }
  return *tmp->node;
}

Token*
peek_last ()
{
  if (last_non_whitespace != NULL)
    return last_non_whitespace;
  return NULL;
}

void
update_top_token (const char *new)
{
  stack_tail->node->length += (int)(new - stack_tail->node->start) - 1;
}

int
unwind_macro (Token pop, macro_store_t *head)
{
  macro_store_t *macro_tmp = head;
  stack_t *stack_tmp = program_stack;

  char *stack_tok, *macro_tok;
  bool first = true;
  TokenType stack_type = TOKEN_ETCSYMBOL;
  while (!is_stack_empty ()
         && (macro_tmp->node.type != TOKEN_MACROBODY)
         && (macro_tmp->node.type != TOKEN_WILDCARDBODY))
    {
      /*  TODO: error checking  */
      /*  TODO: make sure "next" isn't EOF  */
      while (stack_tmp->node->type == TOKEN_WHITESPACE)
        stack_tmp = stack_tmp->next;
      
      if (first)
        {
          stack_tok = malloc (pop.length * sizeof(char));
          sprintf(stack_tok, "%.*s", pop.length, pop.start);
          stack_type = pop.type;
        }
      else
        {
          stack_tok = malloc (stack_tmp->node->length * sizeof(char));
          sprintf(stack_tok, "%.*s", stack_tmp->node->length, stack_tmp->node->start);
          stack_type = stack_tmp->node->type;
        }
      macro_tok = malloc (macro_tmp->node.length * sizeof(char));
      sprintf(macro_tok, "%.*s", macro_tmp->node.length, macro_tmp->node.start);

      #ifdef DEBUG_REPLACE
      printf("\n[ %s , %s ]\n", stack_tok, macro_tok);
      #endif

      if (macro_tmp->node.type == TOKEN_WILDCARDWORD);
      else if ((macro_tmp->node.type == TOKEN_WILDCARDNUMBER)
               && ((stack_type == TOKEN_WORDNUMBER)
                   || (stack_type == TOKEN_MATHNUMBER)))
        ;

      else if (strcmp (stack_tok, macro_tok) != 0)
        return 1;

      free(stack_tok);
      free(macro_tok);
      
      if (!first)
        stack_tmp = stack_tmp->next;
      else
        first = false;
      macro_tmp = macro_tmp->next;
    }
  return 0;
}
