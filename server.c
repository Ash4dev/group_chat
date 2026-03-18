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

  network_task_t tasks[MAX_TASK_QUEUE_SIZE] = {0};
  int task_idx = 0;

  accepted_peer_conn_t *connection;
  while (1) {

    if (tasks[task_idx].is_active) {
      sleep(AVERAGE_CONNECTION_DURATION);
      continue;
    }

    // NOTE: blocking: single prod / no other prod / no cons affected
    if (!(connection = accept_incoming_connection(server_sock_fd))) {
      continue;
    }

    tasks[task_idx].connection = connection;

    // NOTE: only submit in the accept loop
    thread_pool_submit(&pool, tasks + task_idx);
    task_idx = (task_idx + 1) %
               MAX_TASK_QUEUE_SIZE; // NOTE: loop around excess traffic
  }

  thread_pool_destroy(&pool);

  for (int i = 0; i < MAX_TASK_QUEUE_SIZE; ++i) {
    analyze_task(tasks + i);
    record_task(tasks + i);
  }

  main_return_value = EXIT_SUCCESS;
cleanup:

  log_event("Initiating resource clean up");
  if (server_sock_fd >= 0) {
    close(server_sock_fd);
  }

  if (ip_list) {
    freeaddrinfo(ip_list);
  }

  for (int i = 0; i < MAX_TASK_QUEUE_SIZE; ++i) {
    free(tasks[i].connection);
  }

  log_event("Resource clean up complete");
  return main_return_value;
}
