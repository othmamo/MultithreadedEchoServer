#include<stdlib.h>
#include<stdio.h>
#include <semaphore.h>
#include<err.h>
#include "shared_queue.h"

struct queue{
  struct queue* next;
  int value;
};

struct shared_queue* new_shared_queue(void)
{
    struct shared_queue* res = malloc(sizeof(struct shared_queue));
    if(sem_init(&(res->lock),0,1) == -1 || sem_init(&(res->size),0,0)==-1)
        errx(EXIT_FAILURE, "sem_init()");
    res->store=NULL;
    return res;
}

void shared_queue_push(struct shared_queue *queue, int val)
{
    sem_wait(&(queue->lock));
    struct queue* q = malloc(sizeof(struct queue));
    q->value = val;
    if(queue->store == NULL)
        q->next = q;
    else
    {
        q->next = queue->store->next;
        queue->store->next = q;
    }
    queue->store = q;
    sem_post(&(queue->size));
    sem_post(&(queue->lock));
}

int shared_queue_pop(struct shared_queue *queue)
{
    sem_wait(&(queue->size));
    sem_wait(&(queue->lock));
    if(queue->store == NULL)
    {
      errx(EXIT_FAILURE, "Queue is empty, pop is impossible");
    }
    struct queue* q = queue->store->next;
    int res = q->value;
    if(q->next == q)
        queue->store=NULL;
    else
        queue->store->next = q->next;
    free(q);
    sem_post(&(queue->lock));
    return res;
}

void shared_queue_destroy(struct shared_queue *queue)
{
    if(queue!=NULL)
    {
      struct queue *temp;
      while(queue->store != NULL)
      {
        temp = queue->store;
        queue->store = queue->store->next;
        free(temp);
      }
      free(queue);
      sem_destroy(&(queue->lock));
      sem_destroy(&(queue->size));
    }
}
