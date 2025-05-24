#include <stdio.h>

#include "dynamic_array.h"
#include "find.h"

int main(int argc, char *argv[]) {
  da_str_t repos = find_git((argc > 1) ? argv[1] : ".");
  da_foreach(char *, r, &repos) { printf("%s\n", *r); }

  da_free(&repos);

  return EXIT_SUCCESS;
}
