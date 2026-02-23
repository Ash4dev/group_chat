// NOTE: keep it working & simple

#ifndef GRP_CHAT_UTILS
#define GRP_CHAT_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>

#define PROG_SUCCESS 0
#define PROG_FAILURE -1

void log_error(const char *message);
void log_event(const char *message);
void log_output(const char *message);

int obtain_ip_list(const char *hostname, const char *portno,
                   const struct addrinfo *hints,
                   struct addrinfo **ptr_to_ip_list);

int find_valid_connection_address(struct addrinfo *ip_list,
                                  struct addrinfo **found_connection);

#endif // GRP_CHAT_UTILS
