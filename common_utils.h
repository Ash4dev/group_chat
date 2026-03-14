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

#define SERVER_PORT "8080"
#define BUFFER_SIZE 8 * 1024

#define PROG_SUCCESS 0
#define PROG_FAILURE -1

void log_error(const char *message);
void log_event(const char *message);
void log_output(const char *message);

int obtain_ip_list(const char *hostname, const char *portno,
                   const struct addrinfo *hints,
                   struct addrinfo **ptr_to_ip_list);

ssize_t receive_all(const int fd, void *incoming_buffer,
                    size_t incoming_buffer_size);
ssize_t receive_byte_stream(const int fd, char *buffer);

ssize_t send_all(const int fd, const void *outgoing_buffer,
                 size_t outgoing_buffer_size);
ssize_t send_byte_stream(const int fd, const char *buffer, size_t buffer_size);

#endif // GRP_CHAT_UTILS
