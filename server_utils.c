/*
 * File: server_utils.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-13
 * Description:
 * NOTE: keep it working & simple
 */

#include "server_utils.h"
#include "common_utils.h"
#include <stdio.h>
#include <unistd.h>

int create_valid_server_socket(const struct addrinfo *ip_list) {
  for (const struct addrinfo *itr = ip_list; itr != NULL; itr = itr->ai_next) {
    if (itr->ai_addr == NULL)
      continue;
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

accepted_client_socket_t *accept_incoming_connection(int server_sock_fd) {
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size = sizeof client_addr;
  int client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr,
                              &client_addr_size);
  if (client_sock_fd < 0) {
    log_error("Could NOT accept client_connection");
    return NULL;
  }

  accepted_client_socket_t *ptr =
      (accepted_client_socket_t *)malloc(sizeof(accepted_client_socket_t));
  ptr->client_socket_addr = client_addr;
  ptr->client_socket_fd = client_sock_fd;
  return ptr;
}

void client_interaction(int client_socket_fd, int server_socket_fd) {
  char input_buffer[BUFFER_SIZE];
  char output_buffer[BUFFER_SIZE];

  while (1) {
    ssize_t received_byte_count =
        receive_byte_stream(client_socket_fd, (char *)&input_buffer);
    if (received_byte_count <= 0) {
      goto cleanup;
    }

    ssize_t output_byte_count =
        generate_output((const char *)&input_buffer, (char *)&output_buffer);
    if (output_byte_count <= 0) {
      goto cleanup;
    }

    ssize_t sent_byte_count = send_byte_stream(
        client_socket_fd, (const void *)&output_buffer, output_byte_count);
    if (sent_byte_count <= 0) {
      goto cleanup;
    }
  }

cleanup:
  close(client_socket_fd);
}
