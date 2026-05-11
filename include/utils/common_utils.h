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

/**
 * @brief Receives a specific amount of data from a socket by looping recv calls.
 * SOCK_STREAM data arrives in multiple fragments than a single block.
 *
 * @param[in] fd                   The file descriptor of the connected socket.
 * @param[out] incoming_buffer     Pointer to the memory where data will be stored.
 * @param[in] incoming_buffer_size Total number of bytes to receive.
 * 
 * @return The total number of bytes received till EOF returned. (complete / partial)
 * @retval -1 if the connection error occurred.
 */
ssize_t receive_exact(const int fd, char *incoming_buffer,
                    size_t incoming_buffer_size);

/**
 * @brief Receives a length-prefixed (4-bytes) complete message from a TCP stream. No partial reads.
 
 * @param[in]  fd                  The file descriptor of the connected socket.
 * @param[out] buffer              Pointer to the destination buffer for the payload.
 * 
 * @return The number of payload bytes received (excluding the null terminator),
 * @retval -1 if an error occurred, the connection was closed prematurely,
 *         or the incoming message exceeds the buffer capacity.
 */
ssize_t receive_byte_stream(const int fd, char *buffer);

/**
 * @brief Sends a specific amount of data from a socket by looping send calls.
 * 
 * @param fd                       The file descriptor of the connected socket.
 * @param outgoing_buffer          Pointer to the data buffer to be sent.
 * @param outgoing_buffer_size     The exact number of bytes to send.
 * 
 * @return The total number of bytes successfully sent.
 *         If the peer closes the connection mid-transfer (`EPIPE`), it returns 
 *         the number of bytes sent before the closure.
 *         Returns -1 if a fatal network error occurs.
 */
ssize_t send_exact(const int fd, const char *outgoing_buffer,
                 size_t outgoing_buffer_size);

/**
 * @brief Sends a length-prefixed (4-bytes) complete message from a TCP stream. No partial reads.
 * 
 * @param fd                      The file descriptor of the connected socket.
 * @param outgoing_buffer         Pointer to the payload data to be sent.
 * @param outgoing_buffer_size    The size of the payload in bytes.
 * 
 * @return The number of bytes of the *actual payload* sent (excluding the 4-byte header). 
 *         Returns -1 if the header could not be fully sent, or if the payload 
 *         could not be fully sent.
 */
ssize_t send_byte_stream(const int fd, const char *buffer, size_t buffer_size);

#endif // GRP_CHAT_UTILS
