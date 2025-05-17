#ifndef COLOR_NONE
#include "ANSI-color-codes.h"
#endif // COLOR_NONE
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
  CURLUcode rc;
  url_t expected;
} test_case_t;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

const test_case_t TEST_CASES[] = {
    // clang-format off
  { .input = "not a valid url",        .rc = CURLUE_MALFORMED_INPUT, .expected = { 0 } },
  { .input = "foo",                    .rc = CURLUE_OK,              .expected = { .scheme = "ssh",   .host = "foo",  .path = "/",        .shorthand =  true } },
  { .input = "https://host/a/b.git",   .rc = CURLUE_OK,              .expected = { .scheme = "https", .host = "host", .path = "/a/b.git", .shorthand = false } },
  { .input = "https://host/a/b",       .rc = CURLUE_OK,              .expected = { .scheme = "https", .host = "host", .path = "/a/b",     .shorthand = false } },
  { .input = "git://host/a/b.git",     .rc = CURLUE_OK,              .expected = { .scheme = "git",   .host = "host", .path = "/a/b.git", .shorthand = false } },
  { .input = "git@host:a/b.git",       .rc = CURLUE_OK,              .expected = { .scheme = "ssh",   .host = "host", .path = "/a/b.git", .shorthand =  true } },
  { .input = "ssh://git@host/a/b.git", .rc = CURLUE_OK,              .expected = { .scheme = "ssh",   .host = "host", .path = "/a/b.git", .shorthand = false } },
    // clang-format on
};

#ifndef COLOR_NONE
#define test_pass printf("%spass%s", BGRN, CRESET)
#define test_fail(field, actual, expected)                                     \
  do {                                                                         \
    printf("%sfail%s (%s%s%s: expected %s%s%s / actual %s%s%s)", BRED, CRESET, \
           CYN, field, CRESET, BGRN, expected, CRESET, BRED, actual, CRESET);  \
    pass = false;                                                              \
  } while (0)
#else // COLOR_NONE
#define test_pass printf("pass")
#define test_fail(field, actual, expected)                                     \
  do {                                                                         \
    printf("fail (%s: expected %s / actual %s)", field, expected, actual);     \
    pass = false;                                                              \
  } while (0)
#endif // COLOR_NONE

char *bool_to_str(bool b) {
  if (b) {
    return "true";
  } else {
    return "false";
  }
}

int main(int argc, char *argv[]) {
  url_t url;
  CURLUcode rc;

  bool pass = true;

  for (int i = 0; i < ARRAY_SIZE(TEST_CASES); i++) {
    test_case_t t = TEST_CASES[i];
    printf("%-30s... ", t.input);

    url = (url_t){0};
    rc = url_from_string(t.input, &url);

    if (t.rc != rc) {
      test_fail("rc", curlucode_to_string(rc), curlucode_to_string(t.rc));
    } else if (rc == CURLUE_OK) {
      if (strcmp(url.scheme, t.expected.scheme) != 0) {
        test_fail("scheme", url.scheme, t.expected.scheme);
      } else if (strcmp(url.host, t.expected.host) != 0) {
        test_fail("host", url.host, t.expected.host);
      } else if (strcmp(url.path, t.expected.path) != 0) {
        test_fail("path", url.path, t.expected.path);
      } else if (url.shorthand != t.expected.shorthand) {
        test_fail("shorthand", bool_to_str(url.shorthand),
                  bool_to_str(t.expected.shorthand));
      } else {
        test_pass;
      }
    } else {
      test_pass;
    }

    printf("\n");
  }

  if (pass) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
