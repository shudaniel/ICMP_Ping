#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>

class PingSocket {
    public :
        PingSocket(char * ipaddr, unsigned int port);
        void close();
    private:
        int sockfd;
};

#endif