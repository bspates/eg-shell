#include "api.h"

size_t curl_callback ( void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct curl_fetch_st *p = (struct curl_fetch_st *) userp;

  p->payload = (char *) realloc(p->payload, p->size + realsize + 1);

  if(p->payload == NULL) {
    fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
    free(p->payload);
    return -1;
  }

  memcpy(&(p->payload[p->size]), contents, realsize);

  p->size += realsize;

  p->payload[p->size] = 0;

  return realsize;
};

CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch) {
  CURLcode rcode;

  fetch->payload = (char *) calloc(1, sizeof(fetch->payload));

  if(fetch->payload == NULL) {
    fprintf(stderr, "ERROR: Failed to allocate payload in curl_fetch_url");
    return CURLE_FAILED_INIT;
  }

  /* init size */
  fetch->size = 0;

  /* set url to fetch */
  curl_easy_setopt(ch, CURLOPT_URL, url);

  /* set calback function */
  curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);

  /* pass fetch struct pointer */
  curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);

  /* set default user agent */
  curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* set timeout */
  curl_easy_setopt(ch, CURLOPT_TIMEOUT, 5);

  /* enable location redirects */
  curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

  /* set maximum allowed redirects */
  curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

  /* fetch the url */
  rcode = curl_easy_perform(ch);

  /* return */
  return rcode;
}

int json_request(char *method, char *url, struct json_object **body) {
  CURL *ch;    
  CURLcode rcode; // curl result code 

  struct curl_fetch_st curl_fetch; // curl fetch struct 
  struct curl_fetch_st *cf = &curl_fetch; // pointer to fetch struct 
  struct curl_slist *headers = NULL; // http headers to send with request

  /* init curl handle */
  if ((ch = curl_easy_init()) == NULL) {
      /* log error */
      fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session");
      /* return error */
      return 1;
  }

  /* set content type */
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");

  /* set curl options */
  curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, method);
  curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(ch, CURLOPT_POSTFIELDS, json_object_to_json_string(*body));

  /* fetch page and capture return code */
  rcode = curl_fetch_url(ch, url, cf);

  /* cleanup curl handle */
  curl_easy_cleanup(ch);

  /* free headers */
  curl_slist_free_all(headers);

   /* free json object */
  json_object_put(*body);

  /* check return code */
  if (rcode != CURLE_OK || cf->size < 1) {
      /* log error */
      fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s",
          url, curl_easy_strerror(rcode));
      /* return error */
      return 2;
  }

  /* check payload */
  if (cf->payload != NULL) {
      *body = json_tokener_parse(cf->payload);
      /* free payload */
      free(cf->payload);
  } else {
      /* error */
      fprintf(stderr, "ERROR: Failed to populate payload");
      /* free payload */
      free(cf->payload);
      /* return */
      return 3;
  }
  return 0;
}

int valid_response(struct lh_table *body_top_level) {
  struct lh_entry *success, *error_message;
  success = lh_table_lookup_entry(body_top_level, "success");
  if(((int)json_object_get_boolean((struct json_object *) success->v)) != 1) {
    error_message = lh_table_lookup_entry(body_top_level, "message");
    fprintf(stderr, "ERROR: Response from server: %s\n", json_object_get_string((struct json_object *) error_message->v));
    return 1;
  }
  return 0;
}

int request_search(char *query, char **suggestion, struct egg **found) {
  int i = 0;
  json_object *body; // json post body
  struct lh_table *response_body_lookup;
  struct lh_table *results;
  struct array_list *eggs;
  struct lh_table *tmp;
  struct egg *egg_tmp = NULL;

  /* url to test site */
  char *url = "http://192.168.1.7:3000/api/search";

  *found = NULL;
  /* create json object for post */
  body = json_object_new_object();

  /* build post data */
  json_object_object_add(body, "query", json_object_new_string(query));

  if(json_request("POST", url, &body) != 0) return 1;

  response_body_lookup = json_object_get_object(body);
  if(valid_response(response_body_lookup) != 0) return 1;

  results = json_object_get_object(
    (struct json_object *) lh_table_lookup_entry(response_body_lookup, "result")->v
  );

  *suggestion = (char *) json_object_get_string(
    (struct json_object *) lh_table_lookup_entry(results, "suggestion")->v
  );
  eggs = json_object_get_array(
    (struct json_object *) lh_table_lookup_entry(results, "eggs")->v
  );

  for(i=eggs->length-1; i >= 0; i--) {
    if(*found == NULL) {
      *found = (struct egg *)malloc(sizeof(struct egg));
      (*found)->next = NULL;
    } else {
      egg_tmp = (struct egg *)malloc(sizeof(struct egg));
      egg_tmp->next = *found;
      *found = egg_tmp;
    }
    tmp = json_object_get_object(
      (struct json_object *) array_list_get_idx(eggs, i)
    );

    (*found)->name = (char *) json_object_get_string(
      (struct json_object *) lh_table_lookup_entry(
        tmp, 
        "name"
      )->v
    );
    (*found)->description = (char *) json_object_get_string(
      (struct json_object *) lh_table_lookup_entry(
        tmp, 
        "description"
      )->v
    );
  }

  return 0;
}
