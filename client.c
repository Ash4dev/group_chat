#include "client_utils.h"
#include "common_utils.h"

int main() {
  int main_return_value = EXIT_FAILURE;
  struct addrinfo *ip_list = NULL;
  int client_sock_fd = -1;

  struct addrinfo hints;
  memset((void *)&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // NOTE: hints.ai_flags != AI_PASSIVE connectable & node: NULL (loopback addr)
  if (obtain_ip_list(NULL, SERVER_PORT, &hints, &ip_list) < 0)
    goto cleanup;

  log_event("Address was resolved");

  client_sock_fd = create_valid_client_socket(ip_list);
  if (client_sock_fd < 0) {
    log_error("Could NOT open a socket");
    goto cleanup;
  }

  log_event("Socket was obtained");
  log_event("Cient-Server connection established");

  int server_interact_check = server_interaction(client_sock_fd);
  if (server_interact_check < 0) {
    log_event("Server interaction was interrupted");
    goto cleanup;
  }

  main_return_value = EXIT_SUCCESS;
cleanup:

  log_event("Initiating resource clean up");
  if (client_sock_fd >= 0) {
    close(client_sock_fd);
  }
  if (ip_list) {
    freeaddrinfo(ip_list);
  }
  log_event("Resource clean up complete");
  return main_return_value;
}
