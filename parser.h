#ifndef parser_h
#define parser_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "intersection.h"

struct omem {
  Object *itemArray;
  size_t itemNum;
} omem;

struct omem read_scene(FILE* json);
#endif
