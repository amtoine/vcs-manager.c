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

int clone(char *path, char *raw_url, char *fetch_protocol,
          char *push_protocol) {
  url_t url;
  CURLUcode rc;

  url = (url_t){0};
  rc = url_from_string(raw_url, &url);
  if (rc != CURLUE_OK || url.url == NULL) {
    return rc;
  }

  url_t fetch = {0};
  fetch.scheme = fetch_protocol;
  fetch.host = url.host;
  fetch.path = url.path;
  url_t push = {0};
  push.scheme = push_protocol;
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

int main(int argc, char *argv[]) {
  shift(argc, argv);
  if (argc == 0) {
    panic("no command\n");
  }

  char root[1024];
  // NOTE: do not add an extra trailing /
  sprintf(root, ROOT, getenv("HOME"));
  struct stat st = {0};
  if (stat(root, &st) == -1) {
    printf("creating root %s\n", root);
    mkdir(root, 0700);
  }

  if (strcmp(argv[0], "list") == 0) {
    shift(argc, argv);
    if (argc > 0) {
      panic("list: too many arguments\n");
    }
    list(root);
  } else if (strcmp(argv[0], "clone") == 0) {
    shift(argc, argv);
    if (argc == 0) {
      panic("clone: not enough arguments\n");
    } else if (argc > 1) {
      panic("clone: too many arguments\n");
    }
    clone(root, argv[0], "https", "ssh");
  } else {
    panic("%s: unknown command\n", argv[0]);
  }

  return EXIT_SUCCESS;
}
