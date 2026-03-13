#ifndef SERVER_UTILS
#define SERVER_UTILS

#include "common_utils.h"

int create_valid_server_socket(const struct addrinfo *ip_list);

typedef struct accepted_client_socket {
  struct sockaddr_storage client_socket_addr;
  int client_socket_fd;
} accepted_client_socket_t;

accepted_client_socket_t *accept_incoming_connection(int served_socket_fd);

#endif // !SERVER_UTILS
