// Client.c - a multithreaded client/server model file transfer system.
// by: Joseph Mitchell licensed under the CREATIVE COMMONS Attribution-NonCommercial-ShareAlike 4.0 International
#include <stdio.h>		//printf
#include <string.h>		//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
int main(int argc , char *argv[]) {		// MAIN run on execution
	int sock, bytesReceived = 0, chunksReceived = 0, totalBytesReceived = 0;	// main var int declarations/definitions
	struct sockaddr_in server;													// main var sockaddr_in declarations
	FILE *fp;																	// main file object defined
	char *filename , server_reply[200], buff[1024], *size;						// main var char declarations
	if ( argc != 4 ) {												// IF the number of arguments is not 2
    	printf("Client Error\n");										// print local error message
        printf("Usage: %s server-ip server-port filename\n", argv[0]);	// print local proper usage
        return 1;														// return main int
    } else if (atoi(argv[2]) < 1 || atoi(argv[2]) > 65535) {		// IF the port number is not 1 - 65535
    	printf("Server Error\n");													// print local error message
        printf("Port entered: %i is invalid for range (1-65535)\n",atoi(argv[1]));	// print proper port range
        return 1;																	// return main int
    }
    server.sin_addr.s_addr = inet_addr(argv[1]);		// load the server address as user input argument1
    if (inet_pton(AF_INET, argv[1], &(server.sin_addr)) == 0) {	// IF the IPv4 address is not valid
    	printf("Server address is not IPv4\n");						// print local nonvalid address message
        return 1;													// return main int
    }
    fp = fopen("server_file.dl", "w");						// create or overwrite the local server_file
    sock = socket(AF_INET , SOCK_STREAM , 0); 			// socket creation using SOCKET_STREAM (TCP) and AF_INET (IPv4)
	if (sock == -1) { printf("Could not create socket"); }		// IF the socket creation returns -1 then no socket exists
	else { printf("Socket created..."); }						// ELSE the socket creation returns -1 then no socket exists
	server.sin_family = AF_INET;						// load the server family as AF_INET (IPv4)	
	server.sin_port = htons(atoi(argv[2]));				// load the server port as user input integer (argument2)
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) { 	// connect to remote server
		perror("Connect failed. Error");	// branch to print local error if conncet fails
		return 1;							// return main int
	}
	puts("Connected!"); 					// print local connection message
	if(send(sock,argv[3],strlen(argv[3]),0) < 0) {	// send filename (argument3) in message to server on connected socket
		puts("Send failed");				// branch to print local error if send fails
		return 1;							// return main int
	}
	if(recv(sock,server_reply,200,0) < 0) { 		// receive the reply from the server
		puts("recv failed");						// branch to print local error if recv fails
	}
	strtok_r (server_reply, ":", &size);	// split string on first delimiter ":" from server_reply to size
	if (atoi(server_reply) == 404 ){		// IF the server sends the "404" reply
		printf ("File not found on server: %s\n", argv[3]);		// local print file not found message
		puts("Exiting Client");									// local print client exit message 
		return 1;												// return main int
	}
	printf("File Found:\t%s @ %iBytes\n", server_reply, atoi(size));	// print local file found info
    memset(buff, '0', sizeof(buff)); 									// write zeros to the buffer character array
   	while((bytesReceived = read(sock, buff, 1024)) > 0) {				// WHILE receiving messages from the server
    		totalBytesReceived = totalBytesReceived + bytesReceived;		// add the message size to the total size
    		chunksReceived++;												// increment the message/chunk counter then local print message info
        	printf("Message %i\tRecv. Bytes %d  \tTotal Bytes %d\n",chunksReceived,bytesReceived,totalBytesReceived);
        	fwrite(buff,1,bytesReceived,fp);								// write the buffer filled with bytes recieved to file pointer
        	if(totalBytesReceived == atoi(size)) {					//IF the total bytes recieved equals the number intended to send
        		printf("Total messages received %d\n",chunksReceived);	// print local total message recieved information
        		break;													// end the while loop
    		}
    }
    if(bytesReceived < 0) { printf("\n Read Error \n"); }	//IF bytesRecieved is less than zero print that there is a read error
    fflush(stdout);	// flush the file writes
	close(sock);	//close the server socket 
	return 0;		// return main
}
