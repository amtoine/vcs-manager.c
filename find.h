#pragma once

#include <stdlib.h>

typedef struct {
  char **items;
  size_t size;
  size_t capacity;
} da_str_t;

da_str_t find_git(const char *path);
