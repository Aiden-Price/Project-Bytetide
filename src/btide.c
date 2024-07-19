/*
 ============================================================================
 Name        : btide.c
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "config.h"
#include "package.h"

#define BUFFER_SIZE 5520

// Structure to handle connection details
typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;

//
// PART 2
//

// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int signum) {
    // Exit the program with the signal number
    exit(signum); 
}

void * process(void * ptr)
{
	char * buffer;
	int len;
	connection_t * conn;
	long addr = 0;

	if (!ptr) pthread_exit(0);
	conn = (connection_t *)ptr;

	/* read length of message */
	read(conn->sock, &len, sizeof(int));
	if (len > 0)
	{
		addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
		buffer = (char *)malloc((len+1)*sizeof(char));
		buffer[len] = 0;

		/* read message */
		read(conn->sock, buffer, len);

		/* print message */
		printf("%d.%d.%d.%d: %s\n",
			(int)((addr      ) & 0xff),
			(int)((addr >>  8) & 0xff),
			(int)((addr >> 16) & 0xff),
			(int)((addr >> 24) & 0xff),
			buffer);
		free(buffer);
	}

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}


void *cs(void *vargp) {
    int *myid = (int *)vargp;
    printf("Started thread cs %d\n", *myid);
    // Appropriate return for a function used with pthreads that has nothing specific to return
    return NULL; 
}

// Thread function to manage server creation and incoming connections
void *create_server(void *vargp) {
	
	Config *c = (Config *)vargp;
    int server_fd;
    struct sockaddr_in address;
    connection_t * connection;
    pthread_t thread;

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Set up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(c->port);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }


    // Continuously accept incoming connections and spawn new threads to handle them
    while (1) {

        connection = (connection_t *)malloc(sizeof(connection_t));
        socklen_t addrlen = sizeof(struct sockaddr);
        if ((connection->sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            free(connection);
            exit(EXIT_FAILURE);
        } else {
			// start a new thread but do not wait for it
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
        }
    }
    	printf("SERVER\n");
}

// Client-side function to connect to peers, send and receive messages
void start_client(PackageList *pkgList, Config *configuration) {

    char input[BUFFER_SIZE];

    // socket set-up
    struct sockaddr_in serv_addr;
    int sock = 0;

    char* peers[100];
    int peercount = 0;

    while (1) {

    	char *t = fgets(input, BUFFER_SIZE, stdin);
    	if (NULL == t) {
    		fprintf(stderr, "Error: Failed to read input\n");
    	} 
        char *token = strtok(input, " ");
        char in_ip[20];
        int in_port;
        
    	if (strcmp(token, "QUIT\n")==0) {

            exit(0);

    	} else if (strcmp(token, "CONNECT")==0) {
            // Handling connect command to establish new connections
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Missing address and port argument\n");
                exit(0);
            } else {
                token[strcspn(token, "\n")] = 0;
            }

            if (token == NULL) {
                printf("Missing address and port argument\n");
                exit(0);
            }

            token = strtok(token, ":");
            strcpy(in_ip, token);
            token = strtok(NULL, ":");
            in_port = atoi(token);

    	    // Create socket file descriptor
    	    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	        perror("Socket creation error");
    	        return;
    	    }

    	    // Configure server address
    	    serv_addr.sin_family = AF_INET;
    	    serv_addr.sin_port = htons(in_port);

    	    // Convert IPv4 and IPv6 addresses from text to binary form
    	    if (inet_pton(AF_INET, in_ip, &serv_addr.sin_addr) <= 0) {
    	        perror("Invalid address/ Address not supported");
    	        return;
    	    }

    		// Connect to the server
    	    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    	        perror("Connection failed");
    	        return;
    	    }
    	    printf("Connection established with peer\n");
    	    peers[peercount] = in_ip;
    	    peercount++;

    	} else if (strcmp(token, "DISCONNECT")==0) {
            // Handling disconnect command to terminate connections
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Missing address and port argument\n");
                exit(0);
            } else {
                token[strcspn(token, "\n")] = 0;
            }

            if (token == NULL) {
                printf("Missing address and port argument\n");
                exit(0);
            }

            token = strtok(token, ":");
            strcpy(in_ip, token);
            token = strtok(NULL, ":");
            in_port = atoi(token);
            // Close the socket
            close(sock); 
            printf("Disconnected from peer\n");

    	} else if (strcmp(token, "ADDPACKAGE") == 0) {
            // Add a package to the list
            token = strtok(NULL, " ");
            token[strcspn(token, "\n")] = 0;
            addPackage(pkgList, configuration->directory, token);

        } else if (strcmp(token, "REMPACKAGE") == 0) {
             // Remove a package from the list
            token = strtok(NULL, " ");
            token[strcspn(token, "\n")] = 0;
            removePackage(pkgList, token);

        } else if (strcmp(token, "PACKAGES\n")==0) {
            // List all packages
    		listPackages(pkgList);

    	} else if (strcmp(token, "PEERS\n")==0) {
            // List all peers

            if (peercount == 0) {
                printf("Not connected to any peers\n");
            } else {
    		    printf("Connected to:\n");
    		    for (int i=0; i<peercount; i++) {
    			    printf("%d. %s\n", peercount, peers[i]);
    		    }
            }

    	} else if (strcmp(token, "FETCH\n")==0) {
            // Fetch command handler
    		printf("Fetching\n");
    	}
    }
    cleanupPackages(pkgList);
}

// Main function to initialize the server and client
int main(int argc, char *argv[]) {
    PackageList pkgList = {NULL, 0};
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    // allow quit via ctrl-C
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error registering signal handler for SIGINT");
        return 1;
    }

    // Create a config object
    Config config;
    int result = parse_config(argv[1], &config);
    if (result != 0) {
        fprintf(stderr, "Failed to parse configuration with error code: %d\n", result);
        return result;
    }

    pthread_t serverthread;
    pthread_create(&serverthread, 0, create_server, (void *)&config);
    start_client(&pkgList, &config);
	pthread_join(serverthread, NULL);
    return 0;
}