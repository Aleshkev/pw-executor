#include "executor.h"

#include "task_manager.h"

using namespace std;

int main(int argc, char *argv[]) {
  TaskManager *manager = new TaskManager();
  manager->read_and_do_commands();
  delete manager;
}
