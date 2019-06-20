
#include <stdlib.h>
#include <stdbool.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

//implementation

typedef struct list_node list;
struct list_node {
  void* data;
  list* next;
};

typedef struct queue_header queue;
struct queue_header {
  list* front;
  list* back;
  int size;
};

bool is_inclusive_segment(list* start, list* end, int i) {
  if(start == NULL && i == 0) return true;
  if(end == NULL || end->next != NULL) return false;
  if((start == end) && start->next == NULL && i == 1) return true;
  
  int k = 1;
  list* s = start;
  while(s != NULL && s != end){
    if(k > i) return false;
    k++;
    s = s->next;
  }

  if(s == NULL || k != i) return false;

  return true;
}

bool is_queue(queue* Q) {
  if (Q == NULL || Q->size < 0) return false;
  return is_inclusive_segment(Q->front, Q->back, Q->size);
}

queue_t queue_new()
{
  queue_t Q = xmalloc(sizeof(queue));
  Q->front = NULL;
  Q->size = 0;
  ENSURES(Q != NULL);
  ENSURES(is_queue(Q));
  return Q;
}

size_t queue_size(queue_t Q)
{
  REQUIRES(Q != NULL);
  REQUIRES(is_queue(Q));
  ENSURES(Q->size >= 0);
  return Q->size;
}

void enq(queue_t Q, void* x)
{
  REQUIRES(is_queue(Q));
  REQUIRES(Q != NULL);
  list* node = xmalloc(sizeof(list));
  node->data = x;

  if(Q->size == 0){
    Q->front = node;
    Q->back = node;
  }
  else{
    Q->back->next = node;
    Q->back = Q->back->next;
  }

  Q->size++;
  ENSURES(is_queue(Q));
}

void* deq(queue_t Q)
{
  REQUIRES(is_queue(Q));
  REQUIRES(Q != NULL);
  REQUIRES(queue_size(Q) > 0);
  void* x = Q->front->data;
  list* temp = Q->front;
  Q->front = Q->front->next;
  free(temp);
  Q->size--;

  ENSURES(is_queue(Q));
  return x;
}

void *queue_peek(queue_t Q, size_t i)
{
  REQUIRES(is_queue(Q));
  REQUIRES(Q != NULL);
  REQUIRES(i < queue_size(Q));
  list* f = Q->front;
  for(size_t k = 0; k < i; k++){
    f = f->next;
  }
  ENSURES(is_queue(Q));
  return f->data;
}

void queue_reverse(queue_t Q)
{
  REQUIRES(is_queue(Q));
  REQUIRES(Q != NULL);
  if(queue_size(Q) <= 1) return;

  list* prev = NULL;
  list* f = Q->front;

  while(f != NULL){
    list* node = f->next;
    f->next = prev;
    prev = f;
    f = node;
  }

  list* new = Q->front;
  Q->front = Q->back;
  Q->back = new;
  ENSURES(is_queue(Q));
}

bool queue_all(queue_t Q, check_property_fn* P)
{
  REQUIRES(Q != NULL);
  REQUIRES(P != NULL);
  REQUIRES(is_queue(Q));
  
  list* f = Q->front;
  while(f != NULL){
    if(!(*P)(f->data)) return false;
    f = f->next;
  }

  return true;
}

void* queue_iterate(queue_t Q, void* base, iterate_fn* F)
{
  REQUIRES(Q != NULL);
  REQUIRES(F != NULL);
  REQUIRES(is_queue(Q));

  if(queue_size(Q) == 0) return base;
  list* f = Q->front;
  void* B = base;

  while(f != NULL){
    B = (*F)(B, f->data);
    f = f->next;
  }
  return B;
}

void queue_free(queue *Q, free_fn* F)
{
  REQUIRES(Q != NULL);
  REQUIRES(is_queue(Q));
  REQUIRES(F != NULL);
  
  list *f = Q->front;
  while (f != NULL)
  {
  	list* temp = f->next;
  	if (F != NULL) (*F)(f->data);
  	free(f);
  	f = temp;
  }
  free(Q);
}