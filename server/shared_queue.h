# ifndef SHARED_QUEUE_H_
# define SHARED_QUEUE_H_

# include <semaphore.h>
# include <stdlib.h>


struct shared_queue {
  sem_t lock, size;
  struct queue *store;
};

struct shared_queue* new_shared_queue(void);

void shared_queue_push(struct shared_queue *queue, int val);

int shared_queue_pop(struct shared_queue *queue);

void shared_queue_destroy(struct shared_queue *queue);

# endif
