#include "executor.h"

#include <unistd.h>

typedef struct task_t {
  int_t id;
  int_t pid;
  String last_stdout_line;
  String last_stderr_line;
} *Task;

enum { MAX_N_TASKS = 1024 };
Task tasks[MAX_N_TASKS];

void statement_run(Array args) {
  Task x;
  printf("Task %li started: pid %li.\n", x->id, x->pid);
}
void statement_out(Task task) {
  printf("Task %li stdout: '%s'.\n", task->id, task->last_stdout_line->text);
}
void statement_err(Task task) {
  printf("Task %li stderr: '%s'.\n", task->id, task->last_stderr_line->text);
}
void statement_kill(Task task) {}
void statement_sleep(int_t milliseconds) { usleep(milliseconds * 1000); }

bool do_command(Array statement) {
  String label = array_get(statement, 0);
  if (string_equals_chars(label, "run")) {
    statement_run(statement);
  } else if (string_equals_chars(label, "out")) {
    if (statement->length > 2) error("expected only task id");
    int_t task_id = string_to_int(array_get(statement, 1));
    statement_out(tasks[task_id]);
  } else if (string_equals_chars(label, "err")) {
    if (statement->length > 2) error("expected only task id");
    int_t task_id = string_to_int(array_get(statement, 1));
    statement_err(tasks[task_id]);
  } else if (string_equals_chars(label, "kill")) {
    if (statement->length > 2) error("expected only task id");
    int_t task_id = string_to_int(array_get(statement, 1));
    statement_kill(tasks[task_id]);
  } else if (string_equals_chars(label, "sleep")) {
    if (statement->length > 2) error("expected only one number");
    int_t milliseconds = string_to_int(array_get(statement, 1));
    statement_sleep(milliseconds);
  } else if (string_equals_chars(label, "quit")) {
    if (statement->length > 1) error("expected no arguments");
    return true;
  }

  return false;
}

int main(int argc, char *argv[]) {
  char *s = "hiii hi  sd kd ";

  Array x = string_split(new_string(s));
  for (size_t i = 0; i < x->length; ++i) {
    printf("%s\n", ((String)array_get(x, i))->text);
  }
}
