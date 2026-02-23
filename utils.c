#include "utils.h"

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

int find_valid_connection_address(struct addrinfo *ip_list,
                                  struct addrinfo **found_connection) {
  struct addrinfo *itr = ip_list;
  while (itr != NULL && itr->ai_addr == NULL) {
    itr = itr->ai_next;
  }
  if (itr == NULL) {
    log_error("Could NOT find a connection address");
    return PROG_FAILURE;
  }
  *found_connection = itr;
  return PROG_SUCCESS;
}
