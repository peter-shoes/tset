#include "scanner.h"

#ifndef COMPILER_H
#define COMPILER_H

typedef struct macro_track
  {
    Token macro;
    Token body;
    struct macro_track *next;
  } macro_track_t;

void compile (const char* source, const char *outpath);

#endif
