/*
 * File: server_utils.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-13
 * Description:
 * NOTE: keep it working & simple
 */

#include "server_utils.h"

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
