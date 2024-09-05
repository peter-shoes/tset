#include "scanner.h"

#ifndef STACK_H
#define STACK_H

typedef struct stack
  {
    Token node;
    stack *next;
  } stack_t;

int initStack();
int freeStack();
int pushToken();
Token popToken();
int isStackEmpty();
int getStackSize();

#endif