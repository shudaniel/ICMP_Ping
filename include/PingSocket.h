#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#define __APPLE_USE_RFC_3542

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>
#include <stdio.h>

class PingSocket {
    public:
        PingSocket(char * target, long int ttl);
        void pingForever() const;
    private:
        // References:
        // Echo request ICMP format:  https://en.wikipedia.org/wiki/Ping_(networking_utility)#ICMP_packet
        // Packet format is also defined here: https://www.cymru.com/Documents/ip_icmp.h
        struct echopacket {
            u_int16_t type_and_code;
            u_int16_t checksum;
            u_int16_t id;
            u_int16_t seqnum;
        };

        int sockfd;
        bool useIPv4;
        struct sockaddr_in address;    // IPv4
        struct sockaddr_in6 address6;  // IPv6
        bool GetHostIP(char *hostname);
        uint64_t getCurrentTime() const;
        u_int16_t checksum(struct echopacket packet) const;
};

#endif