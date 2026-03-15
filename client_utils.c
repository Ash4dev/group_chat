/*
 * File: client_utils.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-13
 * Description:
 * NOTE: keep it working & simple
 */

#include "client_utils.h"
#include "common_utils.h"
#include <stdio.h>
#include <unistd.h>

int create_valid_client_socket(const struct addrinfo *ip_list) {
  for (const struct addrinfo *itr = ip_list; itr != NULL; itr = itr->ai_next) {
    if (itr->ai_addr == NULL)
      continue;
    // NOTE: return a socket that matches received server ip config
    int client_sock_fd =
        socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
    if (client_sock_fd < 0) {
      continue;
    }
    int connect_check = connect(client_sock_fd, itr->ai_addr, itr->ai_addrlen);
    if (connect_check == 0) {
      return client_sock_fd;
    }
    close(client_sock_fd);
  }
  return PROG_FAILURE;
}

int server_interaction(int client_socket_fd) {
  char input_buffer[BUFFER_SIZE];
  char output_buffer[BUFFER_SIZE];

  ssize_t receive_byte_count = 0;
  ssize_t output_byte_count = 0;
  ssize_t sent_byte_count = 0;

  // NOTE: client initiates connection with the server
  if ((output_byte_count = generate_output(NULL, output_buffer)) <= 0) {
    return PROG_FAILURE;
  }

  if ((sent_byte_count = send_byte_stream(client_socket_fd, output_buffer,
                                          output_byte_count)) <= 0) {
    return PROG_FAILURE;
  }

  // NOTE: fall back to natural flow interaction
  while (1) {
    if ((receive_byte_count =
             receive_byte_stream(client_socket_fd, input_buffer)) <= 0) {
      return PROG_FAILURE;
    }

    if ((output_byte_count = generate_output(input_buffer, output_buffer)) <=
        0) {
      return PROG_FAILURE;
    }

    if ((sent_byte_count = send_byte_stream(client_socket_fd, output_buffer,
                                            output_byte_count)) <= 0) {
      return PROG_FAILURE;
    }
  }

  return PROG_SUCCESS;
}
