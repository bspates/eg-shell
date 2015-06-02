#include "install.h"

int install(char *target) {
  char* suggestion;
  struct egg *found;
  struct egg *tmp;
  int options = 1;
  int choice = -1;

  if(request_search(target, &suggestion, &found) != 0) return 1;

  if(found == NULL) {
    if(!empty_string(suggestion)) {
      printf("\n\nNo results found for %s -> Try '%s'\n\n", target, suggestion);
      return 0;
    }
    printf("Sorry no matching install packages or suggestions\n");
    return 0;
  }
  printf("\nInstall Scripts Found:\n\n");
  tmp = found;
  while(tmp != NULL) {
    printf("[%d] - %s\n", options, tmp->description);
    tmp = tmp->next;
    options++;
  }
  printf("\nEnter chosen script number or -1 to exit: ");
  choice = (getchar() - 47);
  if(choice < 0) return 0;

  return 0;
}
