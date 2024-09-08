#include "common.h"
#include "stack.h"

typedef struct stack
  {
    Token *node;
    struct stack *next;
  } stack_t;

stack_t *program_stack;
stack_t *stack_tail;

int
initStack ()
{
  if ((program_stack = (stack_t*) malloc (sizeof (stack_t))) == NULL)
    return 1;
  program_stack->node = NULL;
  program_stack->next = NULL;
  stack_tail = program_stack;
  return 0;
}

/*  If you're calling this, the program should be over, and therefore the only
    token left in the stack should be a TOKEN_EOF, so you only need to free
    a single node. This was designed intentionally.  */
void
freeStack ()
{
  free (program_stack);
}

int
pushToken (Token *token)
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
  
}

/*  Note that you should call isStackEmpty() before calling this to
    avoid a segfault in the case that the stack is empty. */
Token
popToken()
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
isStackEmpty ()
{
  if (program_stack->node->type == TOKEN_EOF)
    return 1;
  return 0;
}