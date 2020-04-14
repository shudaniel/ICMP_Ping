#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <chrono>

class PingSocket {
    public:
        PingSocket(char * target);
        void pingForever();
    private:
        int sockfd;
        struct sockaddr_in address;
        bool dnsGetHostIp(char *hostname, struct sockaddr_in *address);
        uint64_t getCurrentTime();
};

#endif