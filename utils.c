#include "utils.h"
#include <netdb.h>
#include <sys/socket.h>

void log_error(const char *message) { fprintf(stderr, "Error: %s\n", message); }
void log_event(const char *message) { fprintf(stdout, "Log: %s\n", message); }
void log_output(const char *message) {
  fprintf(stdout, "Output: %s\n", message);
}

int obtain_ip_list(const char *hostname, const char *portno,
                   const struct addrinfo *hints,
                   struct addrinfo **ptr_to_ip_list) {
  int connection_check = getaddrinfo(hostname, portno, hints, ptr_to_ip_list);
  if (connection_check != 0) {
    // NOTE: gai_strerror only works for getaddrinfo
    log_error(gai_strerror(connection_check));
    *ptr_to_ip_list = NULL;
    return PROG_FAILURE;
  }
  return PROG_SUCCESS;
}

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
