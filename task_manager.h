#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "Constraints.h"
#include "task.h"
#include "threads.h"
#include "util.h"

// Should be in shared memory.
struct TaskManager {
  array<Task, Constraints::MAX_N_TASKS> tasks;
  pthread_mutex_t activity_mutex;  // To synchronize printing to stdout.
 private:
  long next_task_id = 0;

 public:
  TaskManager() { assert_zero(pthread_mutex_init(&activity_mutex, nullptr)); }
  ~TaskManager() { assert_zero(pthread_mutex_destroy(&activity_mutex)); }

  Task *get_task_by_id(size_t task_id) {
    if (task_id >= next_task_id) throw invalid_argument("task not yet created");
    return &tasks.at(task_id);
  }

 private:
  void do_run(vector<string> args) {
    if (next_task_id == tasks.size()) throw invalid_argument("too many tasks");

    Task *task = &tasks[next_task_id];
    task->~Task();
    new (task) Task(next_task_id, -1, &activity_mutex);
    ++next_task_id;

    threads::create_process(task, args);
    threads::create_stdout_watcher(task);
    threads::create_stderr_watcher(task);
    threads::create_status_watcher(task);

    cout << "Task " << task->id << " started: pid " << task->pid << ".\n";
  }
  void do_info(Task *task) { cout << *task << endl; }
  void do_out(Task *task) {
    assert_zero(pthread_mutex_lock(&task->stdout_mutex));
    cout << "Task " << task->id << " stdout: '" << task->last_stdout_line.data()
         << "'.\n";
    assert_zero(pthread_mutex_unlock(&task->stdout_mutex));
  }
  void do_err(Task *task) {
    assert_zero(pthread_mutex_lock(&task->stderr_mutex));
    cout << "Task " << task->id << " stderr: '" << task->last_stderr_line.data()
         << "'.\n";
    assert_zero(pthread_mutex_unlock(&task->stderr_mutex));
  }
  void do_kill(Task *task, int signal = SIGINT) {
    if (!task->is_initialized) return;
    clog << "/kill " << *task << endl;
    int kill_status = kill(task->pid, signal);
    if (errno == ESRCH)  // We probably already killed. the process.
      return;
    assert_sys_ok(kill_status);
  }
  void do_sleep(long milliseconds) {
    assert_sys_ok(usleep(milliseconds * 1000));
  }

  bool do_command(const string &line) {
    stringstream args(line);
    string command;
    args >> command;
    if (command == "") return false;

    if (command == "quit") {
      return true;
    }
    if (command == "run") {
      vector<string> program_args;
      while (args) {
        string program_arg;
        args >> program_arg;
        program_args.push_back(program_arg);
      }
      do_run(program_args);
      return false;
    }
    if (command == "info" || command == "out" || command == "err" ||
        command == "kill" || command == "term") {
      long task_id;
      args >> task_id;
      auto task = get_task_by_id(task_id);

      if (!task->is_initialized) {
        throw invalid_argument("the task is not running");
      }

      if (command == "info") {
        do_info(task);
      } else if (command == "out") {
        do_out(task);
      } else if (command == "err") {
        do_err(task);
      } else if (command == "kill") {
        do_kill(task);
      } else if (command == "term") {
        do_kill(task, SIGKILL);
      }
      return false;
    }
    if (command == "sleep") {
      long milliseconds;
      args >> milliseconds;
      do_sleep(milliseconds);
      return false;
    }
    throw invalid_argument("unknown command");
  }

 public:
  void read_and_do_commands() {
    string line;
    while (getline(cin, line)) {
      assert_zero(pthread_mutex_lock(&activity_mutex));

      try {
        if (do_command(line)) break;
      } catch (invalid_argument e) {
        cerr << "error: " << e.what() << endl;
      }

      assert_zero(pthread_mutex_unlock(&activity_mutex));
    }
  }
};

ostream &operator<<(ostream &o, TaskManager &task_manager) {
  o << "TaskManager {"
    << "\n  ";
  bool first = true;
  for (auto &task : task_manager.tasks) {
    if (!task.is_initialized) continue;
    if (!first) o << ",\n  ";
    first = false;
    o << task;
  }
  return o << "\n}";
}

#endif  // TASK_MANAGER_H
