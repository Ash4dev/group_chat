/*
 * File: server_utils.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-13
 * Description:
 * NOTE: keep it working & simple
 */

#include "server_utils.h"
#include "common_utils.h"

int create_valid_server_socket(const struct addrinfo *ip_list) {
  for (const struct addrinfo *itr = ip_list; itr != NULL; itr = itr->ai_next) {
    if (itr->ai_addr == NULL)
      continue;

    // NOTE: listening socket (ONLY accept, NO data transfer) avl in server
    int server_sock_fd =
        socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
    if (server_sock_fd < 0) {
      continue;
    }

    // NOTE: allow reuse of ip+port after restart
    int yes = 1;
    setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    int bind_check = bind(server_sock_fd, itr->ai_addr, itr->ai_addrlen);
    if (bind_check == 0) {
      return server_sock_fd;
    }
    close(server_sock_fd);
  }
  return PROG_FAILURE;
}

accepted_peer_conn_t *accept_incoming_connection(int server_sock_fd) {
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size = sizeof client_addr;

  // NOTE: active connection TCP socket (ONLY data transfer)
  int peer_conn_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr,
                            &client_addr_size);
  if (peer_conn_fd < 0) {
    log_error("Could NOT accept client_connection");
    return NULL;
  }

  accepted_peer_conn_t *ptr =
      (accepted_peer_conn_t *)malloc(sizeof(accepted_peer_conn_t));
  ptr->client_socket_addr = client_addr;
  ptr->peer_conn_fd = peer_conn_fd;
  return ptr;
}

int client_interaction(int peer_conn_fd) {
  char input_buffer[BUFFER_SIZE];
  char output_buffer[BUFFER_SIZE];

  ssize_t receive_byte_count = 0;
  ssize_t output_byte_count = 0;
  ssize_t sent_byte_count = 0;

  while (1) {
    if ((receive_byte_count =
             receive_byte_stream(peer_conn_fd, input_buffer)) <= 0) {
      goto cleanup;
    }

    if ((output_byte_count =
             generate_output((const char *)input_buffer, output_buffer)) <= 0) {
      goto cleanup;
    }

    if ((sent_byte_count = send_byte_stream(peer_conn_fd, output_buffer,
                                            output_byte_count)) <= 0) {
      goto cleanup;
    }
  }

  return PROG_SUCCESS;
cleanup:
  close(peer_conn_fd);
  return PROG_FAILURE;
}
