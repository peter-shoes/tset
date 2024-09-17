#include "common.h"
#include "stack.h"
#include <stdio.h>

typedef struct stack
  {
    Token *node;
    struct stack *next;
  } stack_t;

stack_t *program_stack;
stack_t *stack_tail;

int
init_stack ()
{
  if ((program_stack = (stack_t*) malloc (sizeof (stack_t))) == NULL)
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
  if ((new_node = (stack_t*) malloc (sizeof (stack_t))) == NULL)
    return 1;
  new_node->node = token;
  new_node->next = NULL;
  stack_tail->next = new_node;
  stack_tail = new_node;
  return 0;
}

/*  Note that you should call is_stack_empty() before calling this to
    avoid a segfault in the case that the stack is empty. */
Token
pop_token()
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
  while (depth >= 0)
    {
      if (tmp->next != NULL)
        {
          tmp = tmp->next;
          if (tmp->node->type != TOKEN_WHITESPACE)
            depth--;
        }
      else
      {
        printf("Error: requesting invalid depth\n");
        break;
      }
    }
  return *tmp->node;
}

/*  This is a really bad thing, but I don't want to re-write this to be a 
    doubly linked list, so whatever I'll take the perf hit.  */
Token
peek_last ()
{
  stack_t *tmp = program_stack;
  Token hold;
  while (tmp->next != NULL)
    {
      if (tmp->node->type != TOKEN_WHITESPACE)
        hold = *tmp->node;
      tmp = tmp->next;
    }
  return hold;
}

void
update_top_token (const char *new)
{
  stack_tail->node->length += (int) (new - stack_tail->node->start)-1;
}