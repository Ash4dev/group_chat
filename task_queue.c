/*
 * File: network_task.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-15
 * Description: Thread Pool implementation
 * NOTE: keep it working & simple
 */

#include "task_queue.h"
#include "server_utils.h"

void thread_pool_init(thread_pool_t *pool, int count) {
  pthread_mutex_init(&pool->mx_queue, NULL);

  sem_init(&pool->sem_empty_cnt, 0, MAX_TASK_QUEUE_SIZE);
  sem_init(&pool->sem_fill_cnt, 0, 0);

  pool->worker_cnt = count;
  pool->workers = (pthread_t *)malloc(pool->worker_cnt * sizeof(pthread_t));

  for (int i = 0; i < pool->worker_cnt; ++i) {
    pthread_create(pool->workers + i, NULL, &execute_task_loop, NULL);
  }

  pool->shutdown = 0;

  pool->tail = pool->head = 0;
}

void thread_pool_submit(thread_pool_t *pool, network_task_t *task) {
  sem_wait(&pool->sem_empty_cnt);
  pthread_mutex_lock(&pool->mx_queue);

  pool->tasks[pool->tail] = task;
  pool->tail = (pool->tail + 1) % MAX_TASK_QUEUE_SIZE;

  pthread_mutex_unlock(&pool->mx_queue);
  sem_post(&pool->sem_fill_cnt);
}

void execute_task(network_task_t *task) {
  client_interaction(task->connection->peer_conn_fd);
}

void thread_pool_execute(thread_pool_t *pool, network_task_t *task) {
  sem_wait(&pool->sem_fill_cnt);
  pthread_mutex_lock(&pool->mx_queue);

  network_task_t *task_to_execute = pool->tasks[pool->head];
  pool->head = (pool->head + 1) % MAX_TASK_QUEUE_SIZE;

  pthread_mutex_unlock(&pool->mx_queue);
  sem_post(&pool->sem_empty_cnt);
  execute_task(task_to_execute);
}

void thread_pool_destroy(thread_pool_t *pool) {
  pthread_mutex_destroy(&pool->mx_queue);

  sem_destroy(&pool->sem_empty_cnt);
  sem_destroy(&pool->sem_fill_cnt);

  for (int i = 0; i < pool->worker_cnt; ++i) {
    pthread_join(pool->workers[i], NULL);
  }

  free(pool->workers);
  pool->workers = NULL;

  pool->shutdown = 1;
}
