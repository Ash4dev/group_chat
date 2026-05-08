#ifndef CLIENT_UTILS
#define CLIENT_UTILS

#include "common_utils.h"

int create_valid_client_socket(const struct addrinfo *ip_list);

ssize_t generate_output(const char *input_buffer, char *output_buffer);
int server_interaction(int client_socket_fd);
#endif // !CLIENT_UTILS
