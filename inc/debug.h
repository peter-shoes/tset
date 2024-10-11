#ifndef DEBUG_H
#define DEBUG_H

#include "chunk.h"
#include "scanner.h"

void disassemble_chunk (Chunk *chunk, const char *name);
// static int simple_instruction(const char* name, int offset);
int disassemble_instruction (Chunk *chunk, int offset);
void debug_token (Token, int *);
#endif
