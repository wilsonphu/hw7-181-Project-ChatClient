#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin() {
    /* TODO */
    return 0;

}

int handle_client_socket() {
    /* TODO */
    return 0;
}

int main(int argc, char **argv) {
    /* TODO */

    // Correct Usage for IP and Port Number
    if (argc != 3){
	fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
	return EXIT_FAILURE;

    }
    const char *ip = argv[1];
    struct in_addr addr;
    // To get IP
    if (inet_pton(AF_INET, ip, &addr) != 1) {
        fprintf(stderr, "Error: Invalid IP address: %s\n", ip);
        return EXIT_FAILURE;
    }
    // To get PORT
    int port;
    if (!parse_int(argv[2], &port, "port number")) {
        return EXIT_FAILURE;
    }
    if (port < 1024 || port > 65535) {
	fprintf(stderr, "Error: The port must in range [1024, 65535].\n");
	return EXIT_FAILURE;
    } 

   
    
    
    return EXIT_SUCCESS;
}
