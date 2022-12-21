#ifndef MIMUW_FORK__TYPES_H_
#define MIMUW_FORK__TYPES_H_

#define auto __auto_type

typedef int64_t int_t;

#define alloc_one(SELF) ((SELF) = calloc(1, sizeof *(SELF)))

#define alloc_n(SELF, N) ((SELF) = calloc((N), sizeof *(SELF)))
#define alloc_n_bytes(SELF, N) ((SELF) = calloc(N, 1))

#define dealloc(SELF) (free(SELF), (SELF) = NULL)

#define destroy(SELF)                 \
  (_Generic((SELF), string            \
            : string_destroy, default \
            : free)(SELF),            \
   (SELF) = NULL)

void error(char *what) { fprintf(stderr, "error: %s\n", what); }

// typedef void *Object;

typedef struct object_t {
  char type;
} *Object;

#define extends_object \
  struct {             \
    char type;         \
  }

char get_type(Object self) { return self->type; }
void assert_type(Object self, char expected_type) {
  if (get_type(self) != expected_type) error("type error");
}
Object as_object(void *self) { return self; }

#endif  // MIMUW_FORK__TYPES_H_
