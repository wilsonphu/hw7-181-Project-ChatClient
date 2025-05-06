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
    
    //Prompt for username
    while(1){
	printf("Enter your user name: ");
	fflush(stdout);
	
	if (!fgets(username, sizeof(username), stdin)){
		fprintf(stderr, "Error: Failed to read username.\n");
		return EXIT_FAILURE;
	}

	size_t len = strlen(username);

	if (len > 0 && username[len - 1] != '\n') {
        	int c;
        	while ((c = getchar()) != '\n' && c != EOF);  
        	printf("Sorry, limit your name to %d characters.\n", MAX_NAME_LEN);
        	continue;
    	}
	if (len>0 && username[len-1] == '\n'){
		username[len-1] = '\0';
		len--;	
	}

	//check if username is empty
	if (len == 0){
		continue;
	}

	if (len>MAX_NAME_LEN){
		printf("Sorrt, limit your name to %d characters.\n", MAX_NAME_LEN);
		continue;
	}
	break;
    }

    //greeting 
	printf("Hello, %s. Let's try to connect to the server.\n",username);
    

    return EXIT_SUCCESS;
}
