#include "scanner.h"

#ifndef COMPILER_H
#define COMPILER_H

typedef struct macro_store
{
  Token node;
  struct macro_store *next;
} macro_store_t;

typedef struct macro_track
{
  macro_store_t *store;
  struct macro_track *next;
} macro_track_t;

void compile (const char *source, const char *outpath);

#endif
