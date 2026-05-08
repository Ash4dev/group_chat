#ifndef SERVER_UTILS
#define SERVER_UTILS

#include "common_utils.h"

int create_valid_server_socket(const struct addrinfo *ip_list);

typedef struct accepted_peer_conn {
  struct sockaddr_storage client_socket_addr;
  int peer_conn_fd;
} accepted_peer_conn_t;

accepted_peer_conn_t *accept_incoming_connection(int served_socket_fd);

ssize_t generate_output(const char *input_buffer, char *output_buffer);
int client_interaction(int peer_conn_fd);

#endif // !SERVER_UTILS
