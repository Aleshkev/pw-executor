//
// Created by franz on 2022-12-19.
//

#ifndef MIMUW_FORK__X_STRING_H_
#define MIMUW_FORK__X_STRING_H_

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>

#include "x_array.h"
typedef struct string {
  extends_object;
  char *text;
  size_t length;
} *String;

void string_set_text(String self, const char *text) {
  dealloc(self->text);
  self->text = strdup(text);
  self->length = strlen(text);
}

String new_string(char *text) {
  String self;
  alloc_one(self);
  self->type = 's';
  string_set_text(self, text);
  return self;
}

String new_string_from_slice(char *text, size_t start, size_t stop) {
  char *slice = strndup(text + start, stop - start + 1);
  String self = new_string(slice);
  dealloc(slice);
  return self;
}

void string_destroy(String self) {
  dealloc(self->text);
  dealloc(self);
}

Array string_split(String self) {
  Array result = new_array(self->length);
  char *s = self->text;
  size_t n = self->length;
  for (size_t i = 0; i < n;) {
    while (i < n && s[i] == ' ') ++i;
    if (i == n) break;
    size_t j = i + 1;
    while (j < n && s[j] != ' ') ++j;
    array_push_back(result, new_string_from_slice(s, i, j));
    i = j;
  }
  return result;
}

bool string_equals_chars(String self, char *chars) {
  return strcmp(self->text, chars) == 0;
}

bool strings_equal(String self, String other) {
  assert_type(self, 's');
  return string_equals_chars(self, other->text);
}

int_t string_to_int(String self) {
  assert_type(self, 's');
  for (size_t i = 0; i < self->length; ++i)
    if (!isdigit(self->text[i])) error("string not a number");
  errno = 0;
  int_t x = strtol(self->text, NULL, 10);
  if (errno != 0) error("number too large");
  return x;
}

#endif  // MIMUW_FORK__X_STRING_H_
