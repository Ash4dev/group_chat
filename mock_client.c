#include "utils.h"

int main() {
  // NOTE: define upfront to enable clean up (goto: avoid repeat manual clean)
  int main_return_value = EXIT_FAILURE;
  struct addrinfo *ip_list = NULL;
  struct addrinfo *found_connection = NULL;
  int client_sock_fd = -1;

  // NOTE: obtain connection address for the server
  struct addrinfo hints;
  memset((void *)&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // NOTE: draw a diagram to determine pass by value/address
  if (obtain_ip_list("www.google.com", "80", &hints, &ip_list) < 0)
    goto cleanup;

  if (find_valid_connection_address(ip_list, &found_connection) < 0)
    goto cleanup;

  log_event("Address resolved");

  // NOTE: establish a client socket
  client_sock_fd =
      socket(found_connection->ai_family, found_connection->ai_socktype,
             found_connection->ai_protocol);
  if (client_sock_fd < 0) {
    log_error("Could NOT open a socket");
    goto cleanup;
  }

  log_event("Socket was obtained");

  // NOTE: establish connection of client socket to server
  int connection_check = connect(client_sock_fd, found_connection->ai_addr,
                                 found_connection->ai_addrlen);

  if (connection_check < 0) {
    log_error("Client-Server connection NOT established");
    goto cleanup;
  }

  log_event("Cient-Server connection established");

  // NOTE: send response to google http server
  const char *http_request = "GET / HTTP/1.1\r\n"
                             "Host: www.google.com\r\n"
                             "Connection: close\r\n"
                             "\r\n";

  ssize_t bytes_sent =
      send(client_sock_fd, (const void *)http_request, strlen(http_request), 0);
  if (bytes_sent < 0) {
    log_error("Could NOT send request to google http server");
    goto cleanup;
  }

  // NOTE: receive response from google
  char incoming_buffer[1024]; // application limit
  memset((void *)incoming_buffer, 0, sizeof incoming_buffer);

  /*
   * NOTE: SOCK_STREAM implies info byte-stream

   * send/recv block calling thread (by default)
   * Response comes in as TCP segments, which are ensured & ordered by TCP
   * Internal TCP buffers exist to store the response from the sender
   * All of the response may NOT fit inside the application buffer (1024 bytes)
   * In that case, response will be processed chunk-wise (no fixed size)
   * Hence, we need to keep receiving until bytest_recvd become zero
  */

  ssize_t bytest_recvd;
  while ((bytest_recvd = recv(client_sock_fd, (void *)incoming_buffer,
                              sizeof(incoming_buffer) - 1, 0)) > 0) {

    incoming_buffer[bytest_recvd] = '\0';
    log_output(incoming_buffer);
    log_output("--------------------------------------------\n\n\n\n");
    memset((void *)incoming_buffer, 0, sizeof incoming_buffer);
  }
  if (bytest_recvd <= 0) {
    if (bytest_recvd == 0)
      log_event("Peer connection closed (EOF)");
    else
      log_error("Invalid number of bytes received");
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
