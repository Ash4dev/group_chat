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

// NOTE: application buffer size
#define BUFFER_SIZE 8 * 1024

#define PROG_SUCCESS 0
#define PROG_FAILURE -1

void log_error(const char *message);
void log_event(const char *message);
void log_output(const char *message);

/**
 * @brief getaddrinfo wrapper with error-handling.
 *
 * @param[in]  hostname    Network address to be connected to.
 *                         - @b ip4: Number-dot notation (e.g., "127.0.0.1").
 *                         - @b ipv6: Hexadecimal notation (e.g., "2001:db8::1").
 *                         - @b name: DNS-resolvable string (e.g., "google.com").
 * @param[in]  service     Port or service name for the connection.
 *                         - @b service: Translated via /etc/services (e.g., "https").
 *                         - @b portno: Direct numeric string (e.g., "443").
 * @param[in]  hints       Filters applied during socket address search:
 *                         - @b ai_family: IP version (@c AF_INET, @c AF_INET6).
 *                         - @b ai_socktype: Socket type (@c SOCK_STREAM, @c SOCK_DGRAM).
 *                           - @c SOCK_STREAM: Connection oriented reliable continuous streams
 *                           - @c SOCK_DGRAM: Discrete message oriented
 *                         - @b ai_protocol: Transport protocol (@c IPPROTO_TCP, @c IPPROTO_UDP, or @c 0 Any).
 *                         - @b ai_flags: Bitwise OR'd options (e.g., @c AI_PASSIVE).
 *
 * @param[out] ptr_to_ip_list Pointer to the linked list of returned addresses.
 *
 * [getaddrinfo](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)
 *
 * @return Status code.
 * @retval  0 Success.
 * @retval -1 Failure (check errno for details).
 */
int obtain_ip_list(const char *hostname, const char *service,
                   const struct addrinfo *hints,
                   struct addrinfo **ptr_to_ip_list);

ssize_t receive_all(const int fd, void *incoming_buffer,
                    size_t incoming_buffer_size);
ssize_t receive_byte_stream(const int fd, char *buffer);

ssize_t send_all(const int fd, const void *outgoing_buffer,
                 size_t outgoing_buffer_size);
ssize_t send_byte_stream(const int fd, const char *buffer, size_t buffer_size);

#endif // GRP_CHAT_UTILS
