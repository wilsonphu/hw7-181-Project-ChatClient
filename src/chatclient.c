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
	/**
	printf("[%s]: ", username);
	fflush(stdout);
**/
	if(!fgets(outbuf, sizeof(outbuf),stdin)){
	       return -1;
	}

	size_t len = strlen(outbuf);
	if (len>0 && outbuf[len - 1] != '\n') {
		        fprintf(stderr, "Sorry, limit your message to 1 line of at most %d characters.\n", MAX_MSG_LEN);
			int c;
			while ((c=getchar()) != '\n' && c != EOF);
			printf("[%s]: ", username);
			fflush(stdout);

			return 0;
	}

	if (len>0 && outbuf[len-1] == '\n'){
		outbuf[len-1]='\0';
		len--;
	}

	//handle blank input
	if (len == 0) {
        printf("[%s]: ", username);
        fflush(stdout);
        return 0;
        }

	//send to server if not blank 
	if (len>0){
		if (send_with_length(client_socket, outbuf, len+1)<0){
			fprintf(stderr, "Error: Failed to send message.%s\n", strerror(errno));
		       	return -1;
		}

	//check for "bye"
	if (strcmp(outbuf, "bye") == 0){
		printf("Goodbye.\n");
		return -1;
	} 		
   
       
	printf("[%s]: ", username);
        fflush(stdout);
	}
	return 0;
}

int handle_client_socket() {
    /* TODO */
	int bytes_received = recv_with_length(client_socket, inbuf, sizeof(inbuf));
	if (bytes_received<0){
		fprintf(stderr, "Warning: Failed to receive incoming message.\n");
		return 0;
	}

	if (bytes_received == 0){
		printf("\nConnection to server has been lost.\n");
		return -1;
	}

	if (strcmp(outbuf, "bye") == 0){
		printf("\nInitiated shutdown.\n");
		return -1;
	}

	printf("%s\n",inbuf);



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
    
    printf("Hello, %s. Let's try to connect to the server.\n",username);
    //Create TCP
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket<0){
	    fprintf(stderr, "Error:Failed to create socket. %s\n", strerror(errno));
	    return EXIT_FAILURE;
    }

    // Prepare server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = addr; 
    server_addr.sin_port = htons(port);

    //Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){
	    fprintf(stderr, "Error: Failed to connect to server. %s\n", strerror(errno));
	    close(client_socket);
	    return EXIT_FAILURE;
    }

    //Receive message from the server
    int bytes_received = recv_with_length(client_socket, inbuf, sizeof(inbuf));
    if (bytes_received < 0){
            fprintf(stderr, "Error: Failed to connect to server. %s\n", strerror(errno));
	    close(client_socket);
	    return EXIT_FAILURE;
    }

    if (bytes_received ==0){
	    fprintf(stderr, "Error:Server closed the connection.\n");
	    close(client_socket);
            return EXIT_FAILURE;
    }

    printf("\n%s\n\n", inbuf);

    //send username to server
    if (send_with_length(client_socket, username, strlen(username)+1)<0){
	    fprintf(stderr, "Error: Failed to send username. %s\n", strerror(errno));
	    close(client_socket);
            return EXIT_FAILURE;
    }

    fd_set fds;
    int max_fd = (client_socket > STDIN_FILENO)? client_socket:STDIN_FILENO;
    printf("[%s]: ", username);
    fflush(stdout);

    while (1){
	    FD_ZERO(&fds);
	    FD_SET(STDIN_FILENO, &fds);
	    FD_SET(client_socket, &fds);

	    if(select(max_fd+1, &fds, NULL, NULL, NULL)<0){
		fprintf(stderr, "Error: select() failed. %s\n",strerror(errno));
		break;
	}
       	
	if (FD_ISSET(STDIN_FILENO, &fds)){
		if(handle_stdin()<0){
		break; 
		}
	}

	if (FD_ISSET(client_socket, &fds)){
                if(handle_client_socket()<0){
                break;
                }
        }
	}

	close(client_socket);    
    	return EXIT_SUCCESS;
}
