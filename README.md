# Csocketdl
A Client/Server TCP socket based remote file downloader writen in C. This code was created for submission to Assignment1 for NET4005 at Carleton University in Fall 2016.

# Compile
    gcc -pthread server.c -o Server
    gcc client.c -o Client

# Usage

    ./Server {port-number}
    ./Client {server-ip} {server-port} {filename}

# Protocol

The Server is running in a finite state waiting for incoming TCP connections.

    SERVER          CLIENT
      |     <SYN      |     Seq=0      Len=0
      |   SYN/ACK>    |     Seq=0      Ack=1       Len=0
      |     ACK>      |     Seq=1      Ack=1       Len=0      *Stream-Start*
      |   <PSH,ACK    |     Seq=1      Ack=1       Len=x1     *x1 is number of char in array for filename
      |     ACK>      |     Seq=1      Ack=x1+1    Len=0      *filename message acknowledge x1+1*
      |   PSH,ACK>    |     Seq=1      Ack=x1+1    Len=x2     *x2 is number of char in array for filename plus byte size
