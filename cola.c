/**
 * @file cola.c
 * @author Gabriela Limonta 10-10385
 * @author John Delgado 10-10196
 *
 * Contiene la implementación de una cola.
 *
 */
#include <assert.h>
#include <stdlib.h>

#include "cola.h"

/**
 * Estructura que representa una posición en la cola.
 * Esta estructura tiene 3 campos;
 * el elemento de la cola, un apuntador a la posición
 * anterior y un apuntador a la posición siguiente.
 */
struct cell {
  void * cont;
  struct cell * prev;
  struct cell * next;
};

/**
 * Estructura que representa una cola.
 * Esta estructura tiene 3 campos;
 * el tamaño de la cola, un apuntador a la primera
 * posición y un apuntador a la ultima posición.
 */
struct deque {
  int size;
  struct cell * first;
  struct cell * last;
};

/**
 * Se encarga de retornar una cola vacia.
 * @return retorna una cola vacía.
 */
Deque empty_deque() {
  struct deque * d = malloc(sizeof(struct deque));
  d->size = 0;
  d->first = NULL;
  d->last = NULL;
  return d;
}

/**
 * Se encarga de insertar al inicio de la cola un elemento
 * @param d cola donde se va a insertar el elemento.
 * @param elem elemento que se va a insertar en la cola.
 */
void push_front_deque(Deque d, void * elem) {
  struct cell * c = malloc(sizeof(struct cell));
  c->cont = elem;
  c->prev = NULL;
  c->next = NULL;

  // Caso borde cuando la lista sea vacia.
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

/**
 * Se encarga de insertar al final de la cola un elemento
 * @param d cola donde se va a insertar el elemento.
 * @param elem elemento que se va a insertar en la cola.
 */
void push_back_deque(Deque d, void * elem) {
  struct cell * c = malloc(sizeof(struct cell));
  c->cont = elem;
  c->prev = NULL;
  c->next = NULL;

  // Caso borde cuando la lista sea vacia.
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

/**
 * Se encarga de sacar del inicio de la cola un elemento.
 * @param d cola de donde se va a sacar el elemento.
 * @return elemento que se sacó de la cola.
 */
void * pop_front_deque(Deque d) {
  assert(0 != d->size);

  struct cell * c = d->first;
  void * ret = c->cont;
  d->size -= 1;
  d->first = d->first->next;
  free(c);
  return ret;
}

/**
 * Se encarga de sacar del final de la cola un elemento.
 * @param d cola de donde se va a sacar el elemento.
 * @return elemento que se sacó de la cola.
 */
void * pop_back_deque(Deque d) {
  assert(0 != d->size);

  struct cell * c = d->last;
  void * ret = c->cont;
  d->size -= 1;
  d->last = d->last->prev;
  free(c);
  return ret;
}

/**
 * Se encarga de retornar el tamaño de la cola.
 * @param d cola de la cual se va a retornar su tamaño.
 * @return tamaño de la cola.
 */
int length_deque(Deque d) {
  return d->size;
}
