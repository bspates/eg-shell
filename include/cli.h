#include <string.h>
#include <stdio.h>

#include "install.h"

#define NUM_COMMANDS 1

int valid_command(char *command);
int valid_target(char *target);
int sanitize_target(char *dirty_target, char **clean_target);
int parse_command(int num_arg, char *command);
int parse_target(int num_arg, char *target, char **clean_target);
int run_command(int command_index, int arg_num, char *args[]);
