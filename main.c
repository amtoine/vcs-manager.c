#include "url.h"

#include <curl/urlapi.h>
#include <stdio.h>
#include <stdlib.h>

CURLUcode url_to_string(url_t url, char **str) {
  CURLU *curlu = curl_url();
  int ret;

  ret = curl_url_set(curlu, CURLUPART_SCHEME, url.scheme,
                     CURLU_NON_SUPPORT_SCHEME);
  if (ret != CURLUE_OK) {
    return ret;
  }
  ret = curl_url_set(curlu, CURLUPART_HOST, url.host, 0);
  if (ret != CURLUE_OK) {
    return ret;
  }
  ret = curl_url_set(curlu, CURLUPART_PATH, url.path, 0);
  if (ret != CURLUE_OK) {
    return ret;
  }

  curl_url_get(curlu, CURLUPART_URL, str, 0);

  return ret;
}

int main(int argc, char *argv[]) {
  url_t url;
  CURLUcode rc;

  url = (url_t){0};
  rc = url_from_string(argv[1], &url);
  if (rc != CURLUE_OK || url.url == NULL) {
    return rc;
  }

  char *fetch_protocol = "https";
  char *push_protocol = "ssh";

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
  printf("fetch: %s\n", fetch_str);
  printf("push: %s\n", push_str);

  return EXIT_SUCCESS;
}
