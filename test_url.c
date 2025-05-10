#include "url.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

url_t build_url(char *scheme, char *host, char *path) {
  url_t url = {0};
  url.scheme = scheme;
  url.host = host;
  url.path = path;
  return url;
}

typedef struct {
  char *input;
  url_t expected;
} test_case_t;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

const test_case_t TEST_CASES[] = {
    // clang-format off
  { .input = "https://host/a/b.git",   .expected = { .scheme = "https", .host = "host", .path = "/a/b.git" } },
  { .input = "https://host/a/b",       .expected = { .scheme = "https", .host = "host", .path = "/a/b"     } },
  { .input = "git://host/a/b.git",     .expected = { .scheme = "git",   .host = "host", .path = "/a/b.git" } },
  { .input = "ssh://git@host/a/b.git", .expected = { .scheme = "ssh",   .host = "host", .path = "/a/b.git" } },
    // clang-format on
};

int main(int argc, char *argv[]) {
  url_t url;
  CURLUcode rc;

  bool pass = true;

  for (int i = 0; i < ARRAY_SIZE(TEST_CASES); i++) {
    test_case_t t = TEST_CASES[i];
    printf("%-30s... ", t.input);

    url = (url_t){0};
    rc = url_from_string(t.input, &url);

    if (rc != CURLUE_OK) {
      printf("fail (%s)", curlucode_to_string(rc));
      pass = false;
    } else if (strcmp(url.scheme, t.expected.scheme) != 0) {
      printf("fail (%s: %s / %s)", "scheme", url.scheme, t.expected.scheme);
      pass = false;
    } else if (strcmp(url.host, t.expected.host) != 0) {
      printf("fail (%s: %s / %s)", "host", url.host, t.expected.host);
      pass = false;
    } else if (strcmp(url.path, t.expected.path) != 0) {
      printf("fail (%s: %s / %s)", "path", url.path, t.expected.path);
      pass = false;
    } else {
      printf("pass");
    }

    printf("\n");
  }

  if (pass) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
