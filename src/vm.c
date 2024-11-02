#include "vm.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"

#include <stdio.h>

InterpretResult
interpret (const char *source, const char *outpath)
{
  compile (source, outpath);
  return INTERPRET_OK;
}
