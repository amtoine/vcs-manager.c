#include "cmd.h"
#include "dynamic_array.h"
#include "macros.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

process_t cmd_run_async(cmd_t cmd) {
  if (cmd.size < 1) {
    error("Could not run empty command");
    return INVALID_PROCESS;
  }

  pid_t cpid = fork();
  if (cpid < 0) {
    error("Could not fork child process: %s", strerror(errno));
    return INVALID_PROCESS;
  }

  if (cpid == 0) {
    // NOTE: This leaks a bit of memory in the child process.
    cmd_t cmd_null = {0};
    da_append_many(&cmd_null, cmd.items, cmd.size);
    da_append(&cmd_null, NULL);

    if (execvp(cmd.items[0], (char *const *)cmd_null.items) < 0) {
      panic("Could not exec child process for %s: %s", cmd.items[0],
            strerror(errno));
    }
    unreachable("nob_cmd_run_async_redirect");
  }

  return cpid;
}

cmd_error_t process_wait(process_t proc) {
  if (proc == INVALID_PROCESS)
    return (cmd_error_t){.kind = CMD_ERROR_KIND_INVALID_PROCESS, .code = -1};

  for (;;) {
    int wstatus = 0;
    if (waitpid(proc, &wstatus, 0) < 0) {
      return (cmd_error_t){.kind = CMD_ERROR_KIND_COULD_NOT_WAIT,
                           .code = wstatus};
    }

    if (WIFEXITED(wstatus)) {
      int exit_status = WEXITSTATUS(wstatus);
      if (exit_status != 0) {
        return (cmd_error_t){.kind = CMD_ERROR_KIND_NON_ZERO_CODE,
                             .code = exit_status};
      }

      break;
    }

    if (WIFSIGNALED(wstatus)) {
      return (cmd_error_t){.kind = CMD_ERROR_KIND_SIGNAL_TERMINATED,
                           .code = wstatus};
    }
  }

  return (cmd_error_t){.kind = CMD_ERROR_KIND_OK, .code = 0};
}

cmd_error_t cmd_run_sync(cmd_t cmd) {
  process_t p = cmd_run_async(cmd);
  if (p == INVALID_PROCESS) {
    return (cmd_error_t){.kind = CMD_ERROR_KIND_INVALID_PROCESS, .code = -1};
  }
  return process_wait(p);
}
