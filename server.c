// Server.c - a multithreaded client/server model file transfer system.
// by: Joseph Mitchell licensed under the CREATIVE COMMONS Attribution-NonCommercial-ShareAlike 4.0 International
#include <stdio.h>		// printf, fopen
#include <string.h>		// strlen
#include <stdlib.h>		// strlen
#include <sys/socket.h>	// socket
#include <arpa/inet.h>	// inet_addr
#include <unistd.h>		// write
#include <pthread.h>	// for threading linked with lpthread
#define TSIZE 1024		// define transport size in bytes
void *connection_handler(void *);	//the threaded function constructor
volatile int ccount = 0;			// global connection counter
int main(int argc , char *argv[]) {	// MAIN run on execution
	int socketi , client_sock , socksize , *new_sock, chr; 	// main var int declarations
	struct sockaddr_in server , client;						// main var sockaddr_in declarations
	if (argc != 2) {							// IF the number of arguments is not 2
    	printf("Server Error\n");					// print local error
        printf("Usage: %s port-number\n",argv[0]);	// print local proper usage 
        return 1;									// return main int
    } else if (atoi(argv[1]) < 1 || atoi(argv[1]) > 65535) {	// IF the port number is not 1 - 65535
    	printf("Server Error\n");					// print local error and proper port range
        printf("Port entered: %i is invalid for range (1-65535)\n",atoi(argv[1]));
        return 1;									// return main int
    }
	socketi = socket(AF_INET ,SOCK_STREAM,0);		// socket creation using SOCKET_STREAM (TCP) and AF_INET (IPv4)
	if (socketi == -1) { printf("Could not create socket"); }	// IF the socket creation returns -1 then not socket exists
	server.sin_family = AF_INET;			// load the server family as AF_INET (IPv4)
	server.sin_addr.s_addr = INADDR_ANY;	// load the server address as INADDR_ANY (*)
	server.sin_port = htons(atoi(argv[1]));	// load the server port as user input integer (argument1)
	if( bind(socketi,(struct sockaddr *)&server , sizeof(server)) < 0) {	// Bind the server socket to host
		perror("Bind failed. Error: "); // branch to print local error if bind fails
		return 1;						// return main int
	}
	printf("[SERVER] Socket Bound:\t%s:%i TCP\t", inet_ntoa(server.sin_addr), ntohs(server.sin_port));	// print binding message
	listen(socketi,3);								// Listen on the socket the server just bound
	puts("Waiting for incoming connections...");	// print and return waiting message
	socksize = sizeof(struct sockaddr_in);			// get socketaddr length
	while((client_sock = accept(socketi,(struct sockaddr *)&client,(socklen_t*)&socksize))) { 	//WHILE there is SYN send an ACK
		printf("[SERVER][Session %i] Connection SYN Recieved and ACK'd\n",ccount);	// print ACK message and Client Info.
		printf("[SERVER][Session %i] Source IP:\t%s:%i\n",ccount,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		pthread_t connect_thread;	//define a new process thread datastructure 
		new_sock = malloc(1);		//syscall allocate one smallest unit of memory (64-bit = 32bytes)
		*new_sock = client_sock;	//point new_sock to the accepted client connection socket
		if(pthread_create(&connect_thread,NULL,connection_handler,(void*)new_sock) < 0) { 	//Create a new thread of connection_handler
			perror("Could not create thread");	// branch to print local error if bind fails
			return 1;							// return main int
		}
	}
	if (client_sock < 0) {	// IF the client socket is negative (closed/failed)
		perror("Accept failed");	// branch to print local error if bind fails
		return 1;					// return main int
	}
	return 0;	// return main int
}
void *connection_handler(void *socket_desc) {	// connection_handler is called as a result of a client connection (SYN/ACK)
	int sock = *(int*)socket_desc, read_size, cnt = ccount, tid = (unsigned int)pthread_self(), mid = 0; 	// thread var int declarations/definitions
	char *message , client_message[TSIZE], response[TSIZE], str[15];			// thread var char declarations
	FILE *input_filepointer;													// file object defined
	printf("[SERVER][Session %i] Thread ID:\t%i\n",cnt,tid);					// print local thread id for pthread
	while((read_size = recv(sock,client_message,TSIZE,0)) > 0) { 				// WHILE receiving messages from client
		printf("[SERVER][Session %i] File Reqd:\t%s\n", cnt, client_message);	// print local thread id for pthread
		input_filepointer = fopen(client_message, "r");							//read the client selected file 
		if (input_filepointer == NULL) {			// IF the file is not found i.e. NULL
			strcat(response,"404");							// append 404 to response string
  			write(sock , response , strlen(response));		// send response to client
  			printf("[SERVER][Session %i] Not Found\n", cnt);// print local error file not found
		} else { 									// ELSE found, proceed with allocating memory and reading the file
			fseek(input_filepointer, 0, SEEK_END);							// seek to end of file
			strcat(response,client_message);								// append file name to response
			strcat(response,":");											// append client delimiter to response
			sprintf(str, "%d", ftell(input_filepointer));					// convert filesize long into digit store in str
			printf("[SERVER][Session %i] File Size:\t%s Bytes\n", cnt,str);	// print local file details
			strcat(response,str);											// append str to response after delimiter
			fseek(input_filepointer, 0, SEEK_SET);							// seek back to beginning of file
			write(sock,  response , strlen(response));						// send response to client
			sleep(1);														// Wait 1 second
        	while(1) { 											// Inifinite Loop
            	unsigned char buff[TSIZE]={0};						// declare character buffer of Transport Size (TSIZE)
            	int nread = fread(buff,1,TSIZE,input_filepointer);	// read file in chunks of TSIZE bytes into buffer
            	if(nread > 0) {										// IF more than zero bytes are read
            		mid++;												// increment message id for new message
            		write(sock, buff, nread);							// send the buffer to the socket with size of chunck read
            		printf("[SERVER][Session %i] Message %i\t----->\tSent %d Bytes\n",cnt,mid,nread);	// print local message details
            	}
            	if (nread < TSIZE) {									// IF TSIZE is greater than the size of current chunk
					if (feof(input_filepointer))							// IF the input_filepointer is at EndOfFile (EOF)
                    	printf("[SERVER][Session %i] End of file\n",cnt);		// print local end of file message
                		break;													// end infinite loop
                	if (ferror(input_filepointer))						// IF the input_filepointer has read error
                    	printf("[SERVER][Session %i] Error reading\n",cnt);		// print local end of file message
                		break;													// end infinite loop
            	}
        	}
        }
		if(read_size == -1) { perror("recv failed"); }	// IF there is problem with message recv'd from the client print error
		printf("[SERVER][Session %i] Client disconnected\n",cnt);	// print local client disconnect message
		fflush(stdout);												// flush the file writes
		free(socket_desc);											// free the socket passed to thread
		return 0;													// return threaded void
	}
}
