# Csocketdl
A Client/Server TCP socket based remote file downloader writen in C. This code was created for submission to Assignment1 for NET4005 at Carleton University in Fall 2016.

# Compile
    gcc -pthread server.c -o Server
    gcc client.c -o Client

# Usage

    ./Server {port-number}
    ./Client {server-ip} {server-port} {filename}

# Protocol
Example 1: The Server is running in a finite state waiting for incoming TCP connections when the client is launched pointed at a file that exists on the server.
* The session is established, then the client writes the filename character array to the server socket. 
* The server will then write to the client socket the filename and the size in bytes. 
* The server will push all the bytes of the file to the client socket untile the end of file is reached then it will wait. 
* The client will write the recieved data on the socket to a file until it has reached the intended size, then it will terminate the tcp session.
sda

    SERVER          CLIENT
      |     <SYN      |     Seq=0      Len=0
      |   SYN/ACK>    |     Seq=0      Ack=1       Len=0
      |     ACK>      |     Seq=1      Ack=1       Len=0      *Stream-Start*
      |   <PSH,ACK    |     Seq=1      Ack=1       Len=x1     *x1 is number of char in array for filename
      |     ACK>      |     Seq=1      Ack=x1+1    Len=0      *filename message acknowledge x1+1*
      |   PSH,ACK>    |     Seq=1      Ack=x1+1    Len=x2     *x2 is number of char in array for filename plus byte size
      |     <ACK      |     Seq=x1+1   Ack=x2+1    Len=0      *filename and size message acknowledge x2+1*
      |   PSH,ACK>    |     Seq=x2+1   Ack=x1+1    Len=x3     *x3 is file buffer writen upto 1024 byte size
      |     <ACK      |     Seq=x1+1   Ack=x3+x2+1 Len=0      *filename and size message acknowledge x2+1*
                                                -   +   -
      |   PSH,ACK>    |     Seq=LAck   Ack=x1+1    Len=x4     *x4 is next part file buffer writen upto 1024 byte size
      |     <ACK      |     Seq=x1+1   Ack=LAck+x4 Len=0      *filename and size message acknowledge x2+1*
                                                -   +   -
      |   <FIN,ACK    |     Seq=x1+1   Ack=LAck+x4 Len=0      *terminate tcp session
      |     ACK>      |     Seq=LAck*  Ack=x1+2    Len=0      *terminate acknowledge
