#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json/json.h>

struct curl_fetch_st {
  char *payload;
  size_t size;
};

struct egg {
  char *name;
  char *description;
  struct egg *next;
};

size_t curl_callback ( void *contents, size_t size, size_t nmemb, void *userp);
CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch);
int json_request(char *method, char *url, struct json_object **body);
int valid_response(struct lh_table *body_top_level);
int request_search(char *query, char **suggestion, struct egg **found);
