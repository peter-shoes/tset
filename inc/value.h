#ifndef VALUE_H
#define VALUE_H

#include "common.h"

// we are going to need to have some more accurate reps here
typedef double Value;

typedef struct
{
  int capacity;
  int count;
  Value *values;
} ValueArray;

void init_value_array (ValueArray *array);
void write_value_array (ValueArray *array, Value value);
void free_value_array (ValueArray *array);
void printValue (Value value);

#endif