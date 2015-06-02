#include "cli.h"

int main(int argc, char *argv[]) {
  int command_index;
  command_index = parse_command(argc, argv[1]);
  if(command_index < 0) return 1;

  return run_command(command_index, argc, argv);
}
