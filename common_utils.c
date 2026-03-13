#include "common_utils.h"
#include <netdb.h>
#include <stdio.h>
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

int receive_byte_stream(const int fd) {
  char incoming_buffer[RECEIVE_BUFFER_SIZE];

  ssize_t recv_size = 0;
  while ((recv_size = recv(fd, (void *)incoming_buffer,
                           sizeof(incoming_buffer) - 1, 0)) > 0) {
    incoming_buffer[recv_size] = '\0';
    log_output(incoming_buffer);
    log_output("------------------\n\n");
  }

  return ((recv_size == 0) ? PROG_SUCCESS : PROG_FAILURE);
}
