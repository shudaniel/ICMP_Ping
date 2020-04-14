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

#define PING_PKT_SIZE 128


class PingSocket {
    public:
        PingSocket(char * target, long int ttl);
        void pingForever() const;
    private:
        // References:
        // Echo request format:  https://en.wikipedia.org/wiki/Ping_(networking_utility)#ICMP_packet
        // Packet format is also defined here: https://www.cymru.com/Documents/ip_icmp.h
        struct echopacket {
            u_int8_t type;
            u_int8_t code;
            u_int16_t checksum;
            u_int16_t id;
            u_int16_t seqnum;
        };

        int sockfd;
        struct sockaddr_in address;
        bool dnsGetHostIp(char *hostname, struct sockaddr_in *address);
        uint64_t getCurrentTime() const;
        unsigned short int checksum(struct echopacket packet) const;
};

#endif