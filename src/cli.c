#include "cli.h"

const char* commands[] = {
  "install"
};

int valid_command(char *command) {
  int i = 0;
  int comp = 0;
  if(strlen(command) > 10) return -1;
  for(i=0; i < NUM_COMMANDS; i++) {
    comp = memcmp(commands[i], command, strlen(commands[i]));
    if(comp == 0) return i;
  }
  return -1;
}

int valid_target(char *target) {
  if(strlen(target) > 30) return 1;
  return 0;
}

// TODO: Add actual sanitizing
int sanitize_target(char *dirty_target, char **clean_target) {
  // memcpy(clean_target, dirty_target, strlen(dirty_target));
  *clean_target = dirty_target;
  return 0;
}

int parse_command(int num_arg, char *command) {
  int command_index;
  if(num_arg < 2) {
    printf("No command provided");
    return -1;
  }

  command_index = valid_command(command);
  if(command_index < 0) {
    printf("%s is not a supported command\n", command);
    return -1;
  } 

  return command_index;
}

int parse_target(int num_arg, char *target, char **clean_target) {
  if(num_arg < 3) {
    printf("No target provided");
    return 1;
  }

  if(valid_target(target) != 0) return 1;
  if(sanitize_target(target, clean_target) != 0) return 1;
  return 0;
}

int run_command(int command_index, int arg_num, char *args[]) {
  char *target;
  switch(command_index) {
    case 0: //Install
      if(parse_target(arg_num, args[2], &target) != 0) return 1;
      return install(target);
    default:
      fprintf(stderr, "ERROR: No such command\n");
  }
  return 0;
}
