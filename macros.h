#include <stdio.h>
#include <stdlib.h>

#define panic(...)                                                             \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(EXIT_FAILURE);                                                        \
  } while (0)
#define error(...)                                                             \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
  } while (0)
#define info(...)                                                              \
  do {                                                                         \
    fprintf(stderr, "[INFO] "__VA_ARGS__);                                     \
  } while (0)

#define unreachable(message)                                                   \
  do {                                                                         \
    fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message);  \
    exit(EXIT_FAILURE);                                                        \
  } while (0)
