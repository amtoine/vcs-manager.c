#pragma once

#include <curl/curl.h>
#include <stdbool.h>

typedef struct {
  char *url;
  char *scheme;
  char *user;
  char *password;
  char *options;
  char *host;
  char *port;
  char *path;
  char *query;
  char *fragment;
  char *zoneid;
  bool shorthand;
} url_t;

// `.url` will be `NULL` if the URL could not be parsed
CURLUcode url_from_string(char *, url_t *);
CURLUcode url_to_string(url_t url, char **str);
void url_print(url_t);
char *curlucode_to_string(CURLUcode);
