#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct
{
  Chunk* chunk;
  uint8_t* ip; // instruction pointer
  Value stack[STACK_MAX];
  Value* stackTop;
} VM;

typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM ();
void freeVM ();
InterpretResult interpret (const char* source, const char *outpath);
void push (Value value);
Value pop ();

#endif
