#ifndef UTIL_H
#define UTIL_H

#include <cstdarg>
#include <vector>

using namespace std;

// Convert a vector of strings into a null-terminated vector of char *, so that
// it can be passed to execvp()
vector<char *> to_arg_format(vector<string> &v) {
  vector<char *> args;
  args.reserve((v.size() + 1));
  for (size_t i = 0; i < v.size(); ++i) {
    args[i] = &v[i][0];
  }
  args[v.size()] = nullptr;
  return args;
}

// System error.
void sys_err(const char *fmt, ...) {
  va_list fmt_args;

  fprintf(stderr, "ERROR: ");

  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end(fmt_args);
  fprintf(stderr, " (%d; %s)\n", errno, strerror(errno));
  exit(1);
}

#define assert_sys_ok(expr)                                           \
  do {                                                                \
    if ((expr) == -1)                                                 \
      sys_err(                                                        \
          "system command failed: %s\n\tIn function %s() in %s line " \
          "%d.\n\tErrno: ",                                           \
          #expr, __func__, __FILE__, __LINE__);                       \
  } while (0)

#define assert_zero(expr)                                                 \
  do {                                                                    \
    int err = (expr);                                                     \
    if (err != 0)                                                         \
      sys_err("failed: %s\n\tIn function %s() in %s line %d.\n\tErrno: ", \
              #expr, __func__, __FILE__, __LINE__);                       \
  } while (0)

#define assert_not_null(expr)                                             \
  do {                                                                    \
    void *err = (expr);                                                   \
    if (err == nullptr)                                                   \
      sys_err("failed: %s\n\tIn function %s() in %s line %d.\n\tErrno: ", \
              #expr, __func__, __FILE__, __LINE__);                       \
  } while (0)

void set_to_close_on_exec(int fd) {

  assert_sys_ok(fcntl(fd, F_SETFD, FD_CLOEXEC));
}

#endif  // UTIL_H
