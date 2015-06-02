#include "common.h"

bool empty_string(char *string) {
  if(strcmp("", string) != 0) return false;
  return true;
}
