typedef struct deque * Deque;

Deque empty_deque();
void push_front_deque(Deque d, void * elem);
void push_back_deque(Deque d, void * elem);
void * pop_front_deque(Deque d);
void * pop_back_deque(Deque d);
int length_deque(Deque d);
