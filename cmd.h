#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef int process_t;

typedef struct {
  const char **items;
  size_t size;
  size_t capacity;
} cmd_t;

typedef enum {
  CMD_ERROR_KIND_INVALID_PROCESS,
  CMD_ERROR_KIND_COULD_NOT_WAIT,
  CMD_ERROR_KIND_NON_ZERO_CODE,
  CMD_ERROR_KIND_SIGNAL_TERMINATED,
  CMD_ERROR_KIND_OK,
} cmd_error_kind_t;

typedef struct {
  cmd_error_kind_t kind;
  int code;
} cmd_error_t;

#define INVALID_PROCESS (-1)

process_t cmd_run_async(cmd_t cmd);
cmd_error_t process_wait(process_t proc);
cmd_error_t cmd_run_sync(cmd_t cmd);
