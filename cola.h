/**
 * @file cola.h
 * @author Gabriela Limonta 10-10385
 * @author John Delgado 10-10196
 *
 * Contiene la definición de una cola.
 *
 */
typedef struct deque * Deque;

/**
 * Se encarga de retornar una cola vacia.
 * @return retorna una cola vacía.
 */
Deque empty_deque();

/**
 * Se encarga de insertar al inicio de la cola un elemento.
 * @param d cola donde se va a insertar el elemento.
 * @param elem elemento que se va a insertar en la cola.
 */
void push_front_deque(Deque d, void * elem);

/**
 * Se encarga de insertar al final de la cola un elemento.
 * @param d cola donde se va a insertar el elemento.
 * @param elem elemento que se va a insertar en la cola.
 */
void push_back_deque(Deque d, void * elem);

/**
 * Se encarga de sacar del inicio de la cola un elemento.
 * @param d cola de donde se va a sacar el elemento.
 * @return elemento que se sacó de la cola.
 */
void * pop_front_deque(Deque d);

/**
 * Se encarga de sacar del final de la cola un elemento.
 * @param d cola de donde se va a sacar el elemento.
 * @return elemento que se sacó de la cola.
 */
void * pop_back_deque(Deque d);

/**
 * Se encarga de retornar el tamaño de la cola.
 * @param d cola de la cual se va a retornar su tamaño.
 * @return tamaño de la cola.
 */
int length_deque(Deque d);

/**
 * Se encarga de aplicar una funcion a todos los elementos de la lista.
 * @param f funcion a aplicar.
 * @param d lista o cola de elementos.
 */
void mapM_deque(void (*f)(void *), Deque d);

/**
 * Busca elementos en la cola usando un predicado.
 * @param f predicado.
 * @param d cola de elementos.
 * @return elemento de la cola encontrado, null si no lo encontró.
 */
void * find_deque(int (*f)(void *, void *), Deque d, void * datos);

/**
 * Elimina el primer elemento de la cola que cumpla un predicado.
 * @param f predicado.
 * @param d cola de elementos.
 * @return elemento de la cola eliminado, null si no lo encontró.
 */
void * delete_first_deque(int (*f)(void *, void *), Deque d, void * datos);
