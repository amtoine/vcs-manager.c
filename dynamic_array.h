#define da_reserve(da, expected_capacity)                                      \
  do {                                                                         \
    if ((expected_capacity) > (da)->capacity) {                                \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = 256;                                                  \
      }                                                                        \
      while ((expected_capacity) > (da)->capacity) {                           \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items =                                                            \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
    }                                                                          \
  } while (0)

#define da_append(da, item)                                                    \
  do {                                                                         \
    da_reserve((da), (da)->size + 1);                                          \
    (da)->items[(da)->size++] = (item);                                        \
  } while (0)

#define da_append_many(da, new_items, new_items_size)                          \
  do {                                                                         \
    da_reserve((da), (da)->size + (new_items_size));                           \
    memcpy((da)->items + (da)->size, (new_items),                              \
           (new_items_size) * sizeof(*(da)->items));                           \
    (da)->size += (new_items_size);                                            \
  } while (0)

#define da_foreach(Type, it, da)                                               \
  for (Type *it = (da)->items; it < (da)->items + (da)->size; ++it)

#define da_free(da) free((da)->items)
