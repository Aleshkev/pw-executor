#ifndef THREADS_H
#define THREADS_H

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <array>
#include <cstring>
#include <vector>

#include "task_manager.h"
#include "util.h"

using namespace std;

namespace threads {

void watch_stdout_or_stderr(Task *task, int pipe, pthread_mutex_t *mutex,
                            Task::line_t *last_line) {
  auto as_file = fdopen(pipe, "r");
  assert_not_null(as_file);
  while (true) {
    Task::line_t line;
    auto status = fgets(line.data(), line.size(), as_file);
    if (status == nullptr && ferror(as_file) != 0) sys_err("fgets");
    if (status == nullptr) break;

    line[strlen(line.data()) - 1] = '\0';  // Remove '\n'.

    assert_zero(pthread_mutex_lock(mutex));
    *last_line = line;
    assert_zero(pthread_mutex_unlock(mutex));
  }
}

void watch_stdout(Task *task) {
  watch_stdout_or_stderr(task, task->stdout_pipe, &task->stdout_mutex,
                         &task->last_stdout_line);
}

void create_stdout_watcher(Task *task) {
  assert_zero(pthread_create(&task->stdout_watcher, NULL,
                             (void *(*)(void *))watch_stdout, (void *)task));
}

void watch_stderr(Task *task) {
  watch_stdout_or_stderr(task, task->stderr_pipe, &task->stderr_mutex,
                         &task->last_stderr_line);
}

void create_stderr_watcher(Task *task) {
  assert_zero(pthread_create(&task->stderr_watcher, NULL,
                             (void *(*)(void *))watch_stderr, (void *)task));
}

void watch_status(Task *task) {
  int status;
  assert_sys_ok(waitpid(task->pid, &status, WUNTRACED));

  assert_zero(pthread_join(task->stdout_watcher, nullptr));
  assert_zero(pthread_join(task->stderr_watcher, nullptr));

  stringstream message;
  message << "Task " << task->id << " ended: ";
  if (WIFEXITED(status)) {
    message << "status " << WEXITSTATUS(status) << ".\n";
  } else {
    message << "signalled.\n";
  }
  assert_zero(pthread_mutex_lock(task->pending_messages_mutex));
  task->pending_messages->push_back(message.str());
  assert_zero(pthread_mutex_unlock(task->pending_messages_mutex));
}

void create_status_watcher(Task *task) {
  assert_zero(pthread_create(&task->status_watcher, NULL,
                             (void *(*)(void *))watch_status, (void *)task));
}

void wait_for_process(Task *task) {
  assert_zero(pthread_join(task->status_watcher, nullptr));
}

void create_process(Task *task, vector<string> &program_args) {
  int stdout_pipe[2];
  assert_sys_ok(pipe(stdout_pipe));  // Create pipe.
  int stderr_pipe[2];
  assert_sys_ok(pipe(stderr_pipe));  // Create pipe.

  pid_t child_pid = fork();
  if (child_pid == 0) {                                  // The new process.
    assert_sys_ok(close(stdout_pipe[0]));                // Close reading end.
    assert_sys_ok(dup2(stdout_pipe[1], STDOUT_FILENO));  // Replace stdout.
    assert_sys_ok(close(stdout_pipe[1]));                // Close the original.
    assert_sys_ok(close(stderr_pipe[0]));                // Close reading end.
    assert_sys_ok(dup2(stderr_pipe[1], STDERR_FILENO));  // Replace stderr.
    assert_sys_ok(close(stderr_pipe[1]));                // Close the original.

    // Replace process.
    auto args = to_arg_format(program_args);
    assert_sys_ok(execvp(program_args[0].data(), args.data()));
    return;  // Unreachable.
  }

  task->pid = child_pid;
  assert_sys_ok(close(stdout_pipe[1]));  // Close writing end.
  task->stdout_pipe = stdout_pipe[0];    // Save reading end.
  assert_sys_ok(close(stderr_pipe[1]));  // Close writing end.
  task->stderr_pipe = stderr_pipe[0];    // Save reading end.
}

};  // namespace threads

#endif  // THREADS_H
