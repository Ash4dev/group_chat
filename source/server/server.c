/*
 * File: server.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-15
 * Description: -
 * NOTE: keep it working & simple
 */

#include "task_queue.h"

#define CONNECTION_BACKLOG 5

/* NOTE:
 *
 * lscpu: Threads/core: 2, Cores/socket: 6, sockets: 1 -> 12 threads/socket
 * Total CPU bound: 12
 * Total I/O bound: 12*2 = 24
 * expected task: I/O bound > CPU -> (12+24)/2 ~ 18
 */
#define THREAD_COUNT 18

#define AVERAGE_CONNECTION_DURATION 10

int main() {
  // TODO: FIX PROJ DIRS and CMAKE FILE

  int main_return_value = EXIT_FAILURE;
  int server_sock_fd = -1;
  struct addrinfo *ip_list = NULL;

  struct addrinfo hints;
  memset((void *)&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;

  // NOTE: hostname = NULL & hints.ai_flags = AI_PASSIVE make socket bindable
  if (obtain_ip_list(NULL, SERVER_PORT, &hints, &ip_list) < 0)
    goto cleanup;

  log_event("Address resolved");

  server_sock_fd = create_valid_server_socket(ip_list);
  if (server_sock_fd < 0) {
    log_error("Could NOT open a server socket");
    goto cleanup;
  }

  log_event("Server socket was obtained");
  log_event("Server socket successfully bound");

  int listen_check = listen(server_sock_fd, CONNECTION_BACKLOG);
  if (listen_check < 0) {
    log_error("Could NOT listen on the server socket");
    goto cleanup;
  }

  log_event("Server socket listening now");

  // NOTE: concurrency: send/recv are blocking (each client)
  thread_pool_t pool;
  thread_pool_init(&pool, THREAD_COUNT);

  while (1) {

    // NOTE: blocking: single prod / no other prod / no cons affected
    accepted_peer_conn_t *connection;
    if (!(connection = accept_incoming_connection(server_sock_fd))) {
      continue;
    }

    network_task_t *task = malloc(sizeof(network_task_t));
    task->connection = connection;
    task->success_flag = 0;
    task->is_active = 1;
    thread_pool_submit(&pool, task); // NOTE: only submit here
  }

  thread_pool_destroy(&pool);

  main_return_value = EXIT_SUCCESS;
cleanup:

  log_event("Initiating resource clean up");
  if (server_sock_fd >= 0) {
    close(server_sock_fd);
  }

  if (ip_list) {
    freeaddrinfo(ip_list);
  }

  log_event("Resource clean up complete");
  return main_return_value;
}
