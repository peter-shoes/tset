#ifndef VM_H
#define VM_H




#define STACK_MAX 256

typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

InterpretResult interpret (const char *source, const char *outpath);

#endif
