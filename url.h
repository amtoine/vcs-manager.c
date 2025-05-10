#include <curl/curl.h>

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
} url_t;

// `.url` will be `NULL` if the URL could not be parsed
CURLUcode url_from_string(char *, url_t *);
void url_print(url_t);
char *curlucode_to_string(CURLUcode);
