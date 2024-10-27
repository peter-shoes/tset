#include "scanner.h"
#include "compiler.h"

#ifndef STACK_H
#define STACK_H

typedef struct stack
{
  Token *node;
  struct stack *next;
} stack_t;

int init_stack();
void free_stack();
int push_token(Token*);
Token pop_token();
int is_stack_empty();
Token peek_next (int);
Token* peek_last ();
void update_top_token (const char *);
int unwind_macro (Token, macro_store_t *);
#endif