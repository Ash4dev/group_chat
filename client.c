#include "client_utils.h"

int main() {
  // NOTE: define upfront to enable clean up (goto: avoid repeat manual clean)
  int main_return_value = EXIT_FAILURE;
  struct addrinfo *ip_list = NULL;
  int client_sock_fd = -1;

  // NOTE: obtain connection address for the server
  struct addrinfo hints;
  memset((void *)&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // NOTE: draw a diagram to determine pass by value/address
  if (obtain_ip_list(NULL, SERVER_PORT, &hints, &ip_list) < 0)
    goto cleanup;

  log_output("Address was resolved");

  client_sock_fd = create_valid_client_socket(ip_list);
  if (client_sock_fd < 0) {
    log_error("Could NOT open a socket");
    goto cleanup;
  }

  log_output("Socket was obtained");
  log_output("Cient-Server connection established");

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

  /*
   * NOTE: SOCK_STREAM implies info byte-stream

   * send/recv block calling thread (by default)
   * Response comes in as TCP segments, which are ensured & ordered by TCP
   * Internal TCP buffers exist to store the response from the sender
   * All of the response may NOT fit inside the application buffer (1024 bytes)
   * In that case, response will be processed chunk-wise (no fixed size)
   * Hence, we need to keep receiving until bytest_recvd become zero
  */

  int recv_check = receive_byte_stream(client_sock_fd);
  if (recv_check) {
    log_error("Invalid byte size");
    goto cleanup;
  }

  log_output("reached EOF");
  main_return_value = EXIT_SUCCESS;

cleanup:

  log_output("Initiating resource clean up");
  if (client_sock_fd >= 0) {
    close(client_sock_fd);
  }
  if (ip_list) {
    freeaddrinfo(ip_list);
  }
  log_output("Resource clean up complete");
  return main_return_value;
}
