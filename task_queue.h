#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "server_utils.h"
#include <pthread.h>
#include <semaphore.h>

#define MAX_TASK_QUEUE_SIZE 20

typedef struct {
  accepted_peer_conn_t *connection;
  int success_flag;
} network_task_t;

typedef struct {
  // 8 byte aligned members
  pthread_t *workers;
  pthread_mutex_t mx_queue;
  sem_t sem_empty_cnt;
  sem_t sem_fill_cnt;

  // potential custom alignment
  network_task_t *tasks[MAX_TASK_QUEUE_SIZE];

  // 4 byte aligned members
  uint head;
  uint tail;
  int worker_cnt;
  volatile int shutdown;

  /* NOTE:
   *
   * offset of each member should be known always
   * if flexible in between, can NOT know for following
   */
} thread_pool_t;

void thread_pool_init(thread_pool_t *pool, int count);
void thread_pool_submit(thread_pool_t *pool, network_task_t *task);

int thread_pool_execute(thread_pool_t *pool);
void execute_task(network_task_t *task);
void *execute_task_loop(void *args);

void thread_pool_destroy(thread_pool_t *pool);

#endif // !TASK_QUEUE_H
