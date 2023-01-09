#include "executor.h"

#include "task_manager.h"

using namespace std;

int main(int argc, char *argv[]) {
  TaskManager manager;
  manager.read_and_do_commands();

  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
}
