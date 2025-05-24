#define da_append(da, item)                                                    \
  do {                                                                         \
    if ((da)->size >= (da)->capacity) {                                        \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = 256;                                                  \
      } else {                                                                 \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items =                                                            \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
    }                                                                          \
    (da)->items[(da)->size++] = item;                                          \
  } while (0)

#define da_foreach(Type, it, da)                                               \
  for (Type *it = (da)->items; it < (da)->items + (da)->size; ++it)

#define da_free(da) free((da)->items)
