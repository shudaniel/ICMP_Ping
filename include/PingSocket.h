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

#define PING_PKT_SIZE 64

class PingSocket {
    public:
        PingSocket(char * target, long int ttl);
        void pingForever() const;
    private:
        int sockfd;
        struct sockaddr_in address;
        bool dnsGetHostIp(char *hostname, struct sockaddr_in *address);
        uint64_t getCurrentTime() const;
        char * createPingPacket() const;
};

#endif