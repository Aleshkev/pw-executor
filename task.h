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

#include "Constraints.h"
#include "task_manager.h"
#include "util.h"

using namespace std;

struct Task {
  const bool is_initialized;
  long id;
  long pid;
  using line_t = array<char, Constraints::MAX_LINE_LENGTH + 1>;
  line_t last_stdout_line;
  line_t last_stderr_line;

  // These are initialised if is_initialized is true.
  pthread_mutex_t stdout_mutex;
  pthread_mutex_t stderr_mutex;
  pthread_mutex_t *activity_mutex;  // Inherited from TaskManager.

  pthread_t stdout_watcher;
  pthread_t stderr_watcher;
  pthread_t status_watcher;

  int stdout_pipe = -1;
  int stderr_pipe = -1;

  Task(long id, long pid, pthread_mutex_t *activity_mutex)
      : is_initialized(true),
        id(id),
        pid(pid),
        last_stdout_line({'\0'}),
        last_stderr_line({'\0'}),
        activity_mutex(activity_mutex) {
    assert_zero(pthread_mutex_init(&stdout_mutex, NULL));
    assert_zero(pthread_mutex_init(&stderr_mutex, NULL));
  };
  Task() : is_initialized(false){};

  ~Task() {
    if (is_initialized) {
      assert_zero(pthread_mutex_destroy(&stdout_mutex));
      assert_zero(pthread_mutex_destroy(&stderr_mutex));
    }
    memset((void *)this, 0, sizeof *this);
    new (this) Task();
  }
};

ostream &operator<<(ostream &o, Task &task) {
  if (task.is_initialized) {
    assert_zero(pthread_mutex_lock(&task.stdout_mutex));
    assert_zero(pthread_mutex_lock(&task.stderr_mutex));
  }
  o << "Task {.id = " << task.id << ", .pid = " << task.pid
    << ", .last_stdout_line = \"" << task.last_stdout_line.data()
    << "\", .last_stderr_line = \"" << task.last_stderr_line.data() << "\"}";
  if (task.is_initialized) {
    assert_zero(pthread_mutex_unlock(&task.stdout_mutex));
    assert_zero(pthread_mutex_unlock(&task.stderr_mutex));
  }
  return o;
}

#endif  // TASK_H
