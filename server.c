#include "server_utils.h"

#define CONNECTION_BACKLOG 5

int main() {
  int main_return_value = EXIT_FAILURE;
  int server_sock_fd = -1;
  struct addrinfo *ip_list = NULL;

  struct addrinfo hints;
  memset((void *)&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE; // NOTE: actually different from client side code
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;

  if (obtain_ip_list(NULL, SERVER_PORT, &hints, &ip_list) < 0)
    goto cleanup;

  log_output("Address resolved");

  server_sock_fd = create_valid_server_socket(ip_list);
  if (server_sock_fd < 0) {
    log_error("Could NOT open a server socket");
    goto cleanup;
  }

  log_output("Server socket was obtained");
  log_output("Server socket successfully bound");

  int listen_check = listen(server_sock_fd, CONNECTION_BACKLOG);
  if (listen_check < 0) {
    log_error("Could NOT listen on the server socket");
    goto cleanup;
  }

  log_output("Server socket listening now");

  // TODO: accept & jazz -> multi threaded program begins here

  struct sockaddr_storage client_addr; // NOTE: sockaddr_storage IP-agnostic
  socklen_t client_addr_size = sizeof client_addr;
  int client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr,
                              &client_addr_size);

  int recv_check = receive_byte_stream(client_sock_fd);
  if (recv_check) {
    log_error("Invalid byte size");
    goto cleanup;
  }

  log_output("reached EOF");
  main_return_value = EXIT_SUCCESS;

cleanup:

  log_output("Initiating resource clean up");
  if (server_sock_fd >= 0) {
    close(server_sock_fd);
  }

  if (ip_list) {
    freeaddrinfo(ip_list);
  }

  log_output("Resource clean up complete");
  return main_return_value;
}
