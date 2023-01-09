#include "executor.h"

#include "task_manager.h"

using namespace std;

int main(int argc, char *argv[]) {
  // We don't use iostream for stdout, except for debugging functionality.
  cout.sync_with_stdio(false);

  TaskManager manager;
  manager.read_and_do_commands();

  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
}
