/*
 * File: network_task.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-15
 * Description: Thread Pool implementation
 * NOTE: keep it working & simple
 */

#include "task_queue.h"

void thread_pool_init(thread_pool_t *pool, int count) {
  pthread_mutex_init(&pool->mx_queue, NULL);

  sem_init(&pool->sem_empty_cnt, 0, MAX_TASK_QUEUE_SIZE);
  sem_init(&pool->sem_fill_cnt, 0, 0);

  pool->worker_cnt = count;
  pool->workers = (pthread_t *)malloc(pool->worker_cnt * sizeof(pthread_t));

  pool->shutdown = 0;
  pool->tail = pool->head = 0;
  // NOTE: no sync only main thread runs till here

  for (int i = 0; i < pool->worker_cnt; ++i) {
    pthread_create(pool->workers + i, NULL, &execute_task_loop, (void *)pool);
  }
}

void thread_pool_submit(thread_pool_t *pool, network_task_t *task) {
  sem_wait(&pool->sem_empty_cnt);
  pthread_mutex_lock(&pool->mx_queue);

  if (pool->shutdown) {
    pthread_mutex_unlock(&pool->mx_queue);
    sem_post(&pool->sem_empty_cnt); // NOTE: give slot back else CAPACITY LEAK
    return;
  }

  pool->tasks[pool->tail] = task;
  pool->tail = (pool->tail + 1) % MAX_TASK_QUEUE_SIZE;

  pthread_mutex_unlock(&pool->mx_queue);
  sem_post(&pool->sem_fill_cnt);
}

void execute_task(network_task_t *task) {
  client_interaction(task->connection->peer_conn_fd);
}

void *execute_task_loop(void *args) {
  thread_pool_t *pool = ((thread_pool_t *)args);

  int exec_check = 0;
  while (1) {
    // NOTE: NEVER lock inside if outside or vice-versa: DEADLOCK
    if ((exec_check = thread_pool_execute(pool))) {
      break; // NOTE: every thread must have termination condition
    }
  }

  return NULL;
}

int thread_pool_execute(thread_pool_t *pool) {
  sem_wait(&pool->sem_fill_cnt);
  pthread_mutex_lock(&pool->mx_queue);

  int size =
      (pool->tail - pool->head + MAX_TASK_QUEUE_SIZE) % MAX_TASK_QUEUE_SIZE;
  if (pool->shutdown && size == 0) { // NOTE: deadlock prevention check
    pthread_mutex_unlock(&pool->mx_queue);
    return -1;
  }

  network_task_t *task_to_execute = pool->tasks[pool->head];
  pool->head = (pool->head + 1) % MAX_TASK_QUEUE_SIZE;

  pthread_mutex_unlock(&pool->mx_queue);
  sem_post(&pool->sem_empty_cnt);

  // NOTE: task_to_execute: local to thread & not shared & sequential
  execute_task(task_to_execute);
  free(task_to_execute);
  task_to_execute = NULL;
  return 0;
}

void thread_pool_destroy(thread_pool_t *pool) {

  pthread_mutex_lock(&pool->mx_queue);
  pool->shutdown = 1;

  // NOTE: if thread blocked, NEED to explicitly wake up at shutdown
  for (int i = 0; i < pool->worker_cnt; ++i) {
    sem_post(
        &pool->sem_fill_cnt); // NOTE: ARTIFICIAL SIGNALS: just enough to get by
  }
  pthread_mutex_unlock(&pool->mx_queue);

  for (int i = 0; i < pool->worker_cnt; ++i) {
    pthread_join(pool->workers[i], NULL);
  }

  free(pool->workers);
  pool->workers = NULL;

  // NOTE: if cleared before join, workers still may be using shared pool
  pthread_mutex_destroy(&pool->mx_queue);

  sem_destroy(&pool->sem_empty_cnt);
  sem_destroy(&pool->sem_fill_cnt);
}
