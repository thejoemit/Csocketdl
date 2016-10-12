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
    |     <SYN      |
    |   SYN/ACK>    |
    |     ACK>      |
    |               |
