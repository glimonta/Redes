#include <assert.h>
#include <stdlib.h>

#include "cola.h"

struct cell {
  void * cont;
  struct cell * prev;
  struct cell * next;
};

struct deque {
  int size;
  struct cell * first;
  struct cell * last;
};

Deque empty_deque() {
  struct deque * d = malloc(sizeof(struct deque));
  d->size = 0;
  d->first = NULL;
  d->last = NULL;
  return d;
}

void push_front_deque(Deque d, void * elem) {
  struct cell * c = malloc(sizeof(struct cell));
  c->cont = elem;
  c->prev = NULL;
  c->next = NULL;

  if (0 == d->size) {
    d->first = c;
    d->last = c;
  } else {
    d->first->prev = c;
    c->next = d->first;
    d->first = c;
  }

  d->size += 1;
}

void push_back_deque(Deque d, void * elem) {
  struct cell * c = malloc(sizeof(struct cell));
  c->cont = elem;
  c->prev = NULL;
  c->next = NULL;

  if (0 == d->size) {
    d->first = c;
    d->last = c;
  } else {
    d->last->next = c;
    c->prev = d->last;
    d->last = c;
  }

  d->size += 1;
}

void * pop_front_deque(Deque d) {
  assert(0 != d->size);

  struct cell * c = d->first;
  void * ret = c->cont;
  d->size -= 1;
  d->first = d->first->next;
  free(c);
  return ret;
}

void * pop_back_deque(Deque d) {
  assert(0 != d->size);

  struct cell * c = d->last;
  void * ret = c->cont;
  d->size -= 1;
  d->last = d->last->prev;
  free(c);
  return ret;
}

int length_deque(Deque d) {
  return d->size;
}
