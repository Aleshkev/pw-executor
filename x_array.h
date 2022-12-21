#ifndef MIMUW_FORK__X_ARRAY_H_
#define MIMUW_FORK__X_ARRAY_H_

#include <stdio.h>
#include <stdlib.h>

#include "x_types.h"

typedef struct array {
  extends_object;
  Object *elements;
  size_t max_length;
  size_t length;
} *Array;

Array new_array(size_t max_length) {
  Array self;
  alloc_one(self);
  self->type = 'a';
  alloc_n(self->elements, max_length);
  self->length = 0;
  self->max_length = max_length;
  return self;
}

void array_destroy(Array self) {
  dealloc(self->elements);
  dealloc(self);
}

Object array_get(Array self, size_t i) {
  if (i >= self->length) error("index out of range");
  return self->elements[i];
}

void array_set(Array self, size_t i, Object x) {
  if (i >= self->length) error("index out of range");

  self->elements[i] = x;
}

void array_push_back(Array self, Object x) {
  if (self->length >= self->max_length) error("no space for new elements");

  self->elements[self->length] = x;
  ++self->length;
}

void array_pop_back(Array self) {
  if (self->length == 0) error("empty");

  self->elements[self->length - 1] = NULL;
  --self->length;
}

Array as_array(Object self) {
  assert_type(self, 'a');
  return (Array)self;
}

#endif  // MIMUW_FORK__X_ARRAY_H_
