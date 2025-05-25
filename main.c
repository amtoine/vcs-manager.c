#include "cmd.h"
#include "dynamic_array.h"
#include "find.h"
#include "macros.h"
#include "url.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define ROOT "%s/documents/foo"

void list(char *path) {
  da_str_t repos = find_git(path);
  da_foreach(char *, r, &repos) { printf("%s\n", *r); }
  da_free(&repos);
}

int clone(
    /* clang-format off */
  char *path,           // where to clone
  char *raw_url,        // what to clone
  char *fetch_protocol, // how to fetch (defaults to the scheme of the URL if NULL)
  char *push_protocol   // how to push (defaults to the scheme of the URL if NULL)
    /* clang-format on */
) {
  url_t url;
  CURLUcode rc;

  url = (url_t){0};
  rc = url_from_string(raw_url, &url);
  if (rc != CURLUE_OK || url.url == NULL) {
    return rc;
  }

  url_t fetch = {0};
  if (fetch_protocol == NULL) {
    fetch.scheme = url.scheme;
  } else {
    fetch.scheme = fetch_protocol;
  }
  fetch.host = url.host;
  fetch.path = url.path;

  url_t push = {0};
  if (push_protocol == NULL) {
    push.scheme = url.scheme;
  } else {
    push.scheme = push_protocol;
  }
  push.host = url.host;
  push.path = url.path;

  char *fetch_str;
  rc = url_to_string(fetch, &fetch_str);
  if (rc != CURLUE_OK) {
    return rc;
  }
  char *push_str;
  rc = url_to_string(push, &push_str);
  if (rc != CURLUE_OK) {
    return rc;
  }

  char dest[1024];
  // NOTE: the host alreay has a leading /
  sprintf(dest, "%s/%s%s", path, url.host, url.path);

  cmd_t cmd = {0};
  cmd_error_t ret;

  char *git_clone[] = {"git", "clone", fetch_str, dest};
  da_append_many(&cmd, git_clone, 4);
  ret = cmd_run_sync(cmd);
  if (ret.kind != CMD_ERROR_KIND_OK) {
    panic("");
  }
  cmd.size = 0;

  char *git_remote_set_url_fetch[] = {
      "git", "-C", dest, "remote", "set-url", "origin", fetch_str,
  };
  da_append_many(&cmd, git_remote_set_url_fetch, 7);
  ret = cmd_run_sync(cmd);
  if (ret.kind != CMD_ERROR_KIND_OK) {
    panic("");
  }
  cmd.size = 0;

  char *git_remote_set_url_push[] = {
      "git", "-C", dest, "remote", "set-url", "origin", push_str, "--push",
  };
  da_append_many(&cmd, git_remote_set_url_push, 8);
  ret = cmd_run_sync(cmd);
  if (ret.kind != CMD_ERROR_KIND_OK) {
    panic("");
  }

  return 0;
}

#define shift(c, v)                                                            \
  do {                                                                         \
    c--;                                                                       \
    v++;                                                                       \
  } while (0)

void *__pop(int *c, void ***v) {
  void *ret = *v[0];
  *c = *c - 1;
  *v = *v + 1;
  return ret;
}

#define pop(c, v) ({ __pop(&c, (void ***)&v); })

int main(int argc, char *argv[]) {
  shift(argc, argv);
  if (argc == 0) {
    panic("no command\n");
  }

  char *subcommand = pop(argc, argv);

  char root[1024];
  // NOTE: do not add an extra trailing /
  sprintf(root, ROOT, getenv("HOME"));
  struct stat st = {0};
  if (stat(root, &st) == -1) {
    printf("creating root %s\n", root);
    mkdir(root, 0700);
  }

  if (strcmp(subcommand, "list") == 0) {
    if (argc > 0) {
      panic("list: too many arguments\n");
    }
    list(root);
  } else if (strcmp(subcommand, "clone") == 0) {
    char *fetch_protocol = NULL;
    char *push_protocol = NULL;
    char *remote = NULL;
    while (argc > 0) {
      char *arg = pop(argc, argv);
      if (strncmp(arg, "-f", 2) == 0 || strncmp(arg, "--fetch", 7) == 0) {
        char *value = pop(argc, argv);
        if (strncmp(value, "-", 1) == 0) {
          panic("clone: '%s' looks like an option given to %s\n", value, arg);
        }
        if (fetch_protocol != NULL) {
          panic("clone: '%s' has already been set to %s\n", arg,
                fetch_protocol);
        }
        if (strcmp(value, "https") != 0 && strcmp(value, "ssh") != 0) {
          panic("clone: invalid protocol '%s' given to %s\n", value, arg);
        }
        fetch_protocol = value;
      } else if (strncmp(arg, "-p", 2) == 0 || strncmp(arg, "--push", 6) == 0) {
        char *value = pop(argc, argv);
        if (strncmp(value, "-", 1) == 0) {
          panic("clone: '%s' looks like an option given to %s\n", value, arg);
        }
        if (push_protocol != NULL) {
          panic("clone: '%s' has already been set to %s\n", arg, push_protocol);
        }
        if (strcmp(value, "https") != 0 && strcmp(value, "ssh") != 0) {
          panic("clone: invalid protocol '%s' given to %s\n", value, arg);
        }
        push_protocol = value;
      } else if (remote == NULL) {
        remote = arg;
      } else {
        panic("clone: too many arguments\n");
      }
    }
    if (remote == NULL) {
      panic("clone: missing URL\n");
    }
    clone(root, remote, fetch_protocol, push_protocol);
  } else {
    panic("%s: unknown command\n", subcommand);
  }

  return EXIT_SUCCESS;
}
