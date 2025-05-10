#include <curl/curl.h>

#include <stdio.h>
#include <string.h>

#include "url.h"

CURLUcode url_from_string(char *raw_url, url_t *url) {
  CURLUcode rc;
  CURLU *curl = curl_url();
  char *field;

  rc = curl_url_set(curl, CURLUPART_URL, raw_url, CURLU_NON_SUPPORT_SCHEME);
  if (rc != CURLUE_OK) {
    curl_url_cleanup(curl);
    return rc;
  }

#define __url_set(part, member)                                                \
  if (!curl_url_get(curl, part, &field, 0)) {                                  \
    url->member = strdup(field);                                               \
    curl_free(field);                                                          \
  }

  __url_set(CURLUPART_URL, url);
  __url_set(CURLUPART_SCHEME, scheme);
  __url_set(CURLUPART_USER, user);
  __url_set(CURLUPART_PASSWORD, password);
  __url_set(CURLUPART_OPTIONS, options);
  __url_set(CURLUPART_HOST, host);
  __url_set(CURLUPART_PORT, port);
  __url_set(CURLUPART_PATH, path);
  __url_set(CURLUPART_QUERY, query);
  __url_set(CURLUPART_FRAGMENT, fragment);
  __url_set(CURLUPART_ZONEID, zoneid);

  curl_url_cleanup(curl);
  return CURLUE_OK;
}

#define __url_print_field(url, field)                                          \
  do {                                                                         \
    printf("    %-10s:", #field);                                              \
    if (url.field != NULL) {                                                   \
      printf(" %s", url.field);                                                \
    }                                                                          \
    printf("\n");                                                              \
  } while (0)

void url_print(url_t url) {
  printf("{\n");
  __url_print_field(url, url);
  __url_print_field(url, scheme);
  __url_print_field(url, user);
  __url_print_field(url, password);
  __url_print_field(url, options);
  __url_print_field(url, host);
  __url_print_field(url, port);
  __url_print_field(url, path);
  __url_print_field(url, query);
  __url_print_field(url, fragment);
  __url_print_field(url, zoneid);
  printf("}\n");
}

char *curlucode_to_string(CURLUcode rc) {
#define curlucode_to_string_case(code)                                         \
  case code:                                                                   \
    return #code

  switch (rc) {
    curlucode_to_string_case(CURLUE_OK);
    curlucode_to_string_case(CURLUE_BAD_HANDLE);
    curlucode_to_string_case(CURLUE_BAD_PARTPOINTER);
    curlucode_to_string_case(CURLUE_MALFORMED_INPUT);
    curlucode_to_string_case(CURLUE_BAD_PORT_NUMBER);
    curlucode_to_string_case(CURLUE_UNSUPPORTED_SCHEME);
    curlucode_to_string_case(CURLUE_URLDECODE);
    curlucode_to_string_case(CURLUE_OUT_OF_MEMORY);
    curlucode_to_string_case(CURLUE_USER_NOT_ALLOWED);
    curlucode_to_string_case(CURLUE_UNKNOWN_PART);
    curlucode_to_string_case(CURLUE_NO_SCHEME);
    curlucode_to_string_case(CURLUE_NO_USER);
    curlucode_to_string_case(CURLUE_NO_PASSWORD);
    curlucode_to_string_case(CURLUE_NO_OPTIONS);
    curlucode_to_string_case(CURLUE_NO_HOST);
    curlucode_to_string_case(CURLUE_NO_PORT);
    curlucode_to_string_case(CURLUE_NO_QUERY);
    curlucode_to_string_case(CURLUE_NO_FRAGMENT);
    curlucode_to_string_case(CURLUE_NO_ZONEID);
    curlucode_to_string_case(CURLUE_BAD_FILE_URL);
    curlucode_to_string_case(CURLUE_BAD_FRAGMENT);
    curlucode_to_string_case(CURLUE_BAD_HOSTNAME);
    curlucode_to_string_case(CURLUE_BAD_IPV6);
    curlucode_to_string_case(CURLUE_BAD_LOGIN);
    curlucode_to_string_case(CURLUE_BAD_PASSWORD);
    curlucode_to_string_case(CURLUE_BAD_PATH);
    curlucode_to_string_case(CURLUE_BAD_QUERY);
    curlucode_to_string_case(CURLUE_BAD_SCHEME);
    curlucode_to_string_case(CURLUE_BAD_SLASHES);
    curlucode_to_string_case(CURLUE_BAD_USER);
    curlucode_to_string_case(CURLUE_LACKS_IDN);
    curlucode_to_string_case(CURLUE_TOO_LARGE);
    curlucode_to_string_case(CURLUE_LAST);
  }

  return NULL;
}
