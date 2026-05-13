#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "server_utils.h"
#include <pthread.h>
#include <semaphore.h>

#define MAX_TASK_QUEUE_SIZE 20

/**
 * @brief Represents a single network-related task to be processed by a worker thread.
 *
 * @details This structure holds the:
 * - accepted_peer_conn_t *connection: pointer to the accepted peer connection associated with this task.
 * - int success_flag: success flag
 * - int is_active: active/pending flag
 */
typedef struct {
  accepted_peer_conn_t *connection;
  int success_flag;
  int is_active;
} network_task_t;


/**
 * @brief A thread pool managing a circular task queue and worker threads.
 * 
 * Implements SP-MC (single producer - multi consumer) pattern
 *
 * @details This structure holds the:
 * pthread_t *workers: shared resource
 * pthread_mutex_t mx_queue: ownership
 * sem_t sem_empty_cnt: available spot count
 * sem_t sem_fill_cnt: taken-up spot count
 * network_task_t *tasks[MAX_TASK_QUEUE_SIZE]: bounded circular queue
 * size_t head: insertion index
 * size_t tail: extraction index
 * int worker_cnt: number of resources allocated
 * volatile int shutdown: immediate signal of thread pool termination
 */
typedef struct {
  // 8 byte aligned members
  pthread_t *workers;
  pthread_mutex_t mx_queue;
  sem_t sem_empty_cnt;
  sem_t sem_fill_cnt;

  // potential custom alignment
  network_task_t *tasks[MAX_TASK_QUEUE_SIZE];

  // 4 byte aligned members
  size_t head;
  size_t tail;
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
void analyze_task(network_task_t *task);
void record_task(network_task_t *task);
void *execute_task_loop(void *args);

void thread_pool_destroy(thread_pool_t *pool);

#endif // !TASK_QUEUE_H
