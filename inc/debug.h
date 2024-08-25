#ifndef DEBUG_H
#define DEBUG_H

#include "chunk.h"

void disassembleChunk(Chunk* chunk, const char* name);
// static int simpleInstruction(const char* name, int offset);
int disassembleInstruction(Chunk* chunk, int offset);

#endif