#include "scanner.h"

#ifndef STACK_H
#define STACK_H

int init_stack();
void free_stack();
int push_token(Token*);
Token pop_token();
int is_stack_empty();
Token peek_next (int);
Token peek_last ();
void update_top_token (const char *);
#endif