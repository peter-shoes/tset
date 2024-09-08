#include "scanner.h"

#ifndef STACK_H
#define STACK_H

int initStack();
void freeStack();
int pushToken(Token *);
Token popToken();
int isStackEmpty();
int getStackSize();

#endif