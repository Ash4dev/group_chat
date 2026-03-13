/*
 * File: client_utils.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-13
 * Description:
 * NOTE: keep it working & simple
 */

#include "client_utils.h"

int create_valid_client_socket(const struct addrinfo *ip_list) {
  for (const struct addrinfo *itr = ip_list; itr != NULL; itr = itr->ai_next) {
    if (itr->ai_addr == NULL)
      continue;
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
