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

ssize_t receive_all(const int fd, void *incoming_buffer,
                    size_t incoming_buffer_size) {

  ssize_t total_recvd = 0;

  while (total_recvd < incoming_buffer_size) {
    ssize_t recv_byte_count = recv(fd, (char *)(incoming_buffer) + total_recvd,
                                   incoming_buffer_size - total_recvd, 0);

    // NOTE: error / peer closed connection
    if (recv_byte_count <= 0) {
      return -1;
    }

    total_recvd += recv_byte_count;
  }

  return total_recvd;
}

ssize_t receive_byte_stream(const int fd, char *buffer) {

  // NOTE: obtain message size first
  uint32_t incoming_buffer_size = 0;
  ssize_t size_byte_count =
      receive_all(fd, (void *)&incoming_buffer_size, sizeof(uint32_t));

  // NOTE: convert from network to host byte order (endianess check)
  incoming_buffer_size = ntohl(incoming_buffer_size);
  if (size_byte_count <= 0 || incoming_buffer_size + 1 > BUFFER_SIZE) {
    return -1;
  }

  // NOTE: obtain actual message
  ssize_t total_recvd = receive_all(fd, buffer, incoming_buffer_size);
  if (total_recvd <= 0) {
    return -1;
  }
  buffer[total_recvd] = '\0';

  return total_recvd + 1;
}

ssize_t send_all(const int fd, const void *outgoing_buffer,
                 size_t outgoing_buffer_size) {
  ssize_t total_sent = 0;

  while (total_sent < outgoing_buffer_size) {
    ssize_t sent_byte_count =
        send(fd, (const char *)(outgoing_buffer) + total_sent,
             outgoing_buffer_size - total_sent, 0);

    // NOTE: error / peer closed connection
    if (sent_byte_count <= 0) {
      return -1;
    }

    total_sent += sent_byte_count;
  }

  return total_sent;
}

ssize_t send_byte_stream(const int fd, const char *outgoing_buffer,
                         size_t outgoing_buffer_size) {

  // NOTE: send out the message size
  uint32_t len = htonl(outgoing_buffer_size);
  ssize_t size_byte_count = send_all(fd, (void *)&len, sizeof(uint32_t));

  if (size_byte_count <= 0) {
    return -1;
  }

  // NOTE: send the actual message
  ssize_t total_sent =
      send_all(fd, (const void *)(outgoing_buffer), outgoing_buffer_size);
  if (total_sent <= 0) {
    return -1;
  }

  return total_sent;
}
