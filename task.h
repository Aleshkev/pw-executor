#ifndef TASK_H
#define TASK_H

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <array>
#include <cstring>

#include "task_manager.h"
#include "util.h"

using namespace std;

namespace constraints {
[[maybe_unused]] constexpr size_t MAX_COMMAND_LENGTH = 512;
constexpr size_t MAX_N_TASKS = 4096;
constexpr size_t MAX_LINE_LENGTH = 1024;
}  // namespace constraints

struct Task {
  long id;   // Id of the task.
  long pid;  // Pid of the task's process.

  // A line of text.
  using line_t = array<char, constraints::MAX_LINE_LENGTH + 1>;

  // Last line that was printed by the process to stdout, stderr.

  line_t last_stdout_line;
  line_t last_stderr_line;

  // Mutexes to access last_stdout_line, last_stderr_line.

  pthread_mutex_t stdout_mutex;
  pthread_mutex_t stderr_mutex;
  pthread_mutex_t stdout_watcher_mutex;
  pthread_mutex_t stderr_watcher_mutex;
  pthread_mutex_t status_watcher_mutex;

  // Access to TaskManager's data.

  pthread_mutex_t *activity_mutex;
  vector<string> *pending_messages;
  pthread_mutex_t *pending_messages_mutex;

  // The threads waiting for the process's streams and status.

  pthread_t stdout_watcher;
  pthread_t stderr_watcher;
  pthread_t status_watcher;

  // The pipes to communicate with the process.

  int stdout_pipe = -1;
  int stderr_pipe = -1;

  Task(long id, long pid, pthread_mutex_t *activity_mutex,
       vector<string> *pending_messages,
       pthread_mutex_t *pending_messages_mutex)
      : id(id),
        pid(pid),
        last_stdout_line({'\0'}),
        last_stderr_line({'\0'}),
        activity_mutex(activity_mutex),
        pending_messages(pending_messages),
        pending_messages_mutex(pending_messages_mutex) {
    assert_zero(pthread_mutex_init(&stdout_mutex, NULL));
    assert_zero(pthread_mutex_init(&stderr_mutex, NULL));
    assert_zero(pthread_mutex_init(&stdout_watcher_mutex, NULL));
    assert_zero(pthread_mutex_init(&stderr_watcher_mutex, NULL));
    assert_zero(pthread_mutex_init(&status_watcher_mutex, NULL));
  };
  ~Task() {
    assert_zero(pthread_mutex_destroy(&stdout_mutex));
    assert_zero(pthread_mutex_destroy(&stderr_mutex));
    assert_zero(pthread_mutex_destroy(&stdout_watcher_mutex));
    assert_zero(pthread_mutex_destroy(&stderr_watcher_mutex));
    assert_zero(pthread_mutex_destroy(&status_watcher_mutex));
  }
};

// Print the task (for debugging).
ostream &operator<<(ostream &o, Task &task) {
  assert_zero(pthread_mutex_lock(&task.stdout_mutex));
  assert_zero(pthread_mutex_lock(&task.stderr_mutex));
  o << "Task {.id = " << task.id << ", .pid = " << task.pid
    << ", .last_stdout_line = \"" << task.last_stdout_line.data()
    << "\", .last_stderr_line = \"" << task.last_stderr_line.data() << "\"}";
  assert_zero(pthread_mutex_unlock(&task.stdout_mutex));
  assert_zero(pthread_mutex_unlock(&task.stderr_mutex));
  return o;
}

#endif  // TASK_H
