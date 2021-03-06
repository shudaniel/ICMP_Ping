#include "PingSocket.h"

PingSocket::PingSocket(char *target, long int ttl, unsigned int timeoutval)
{
    m_ttl = ttl;
    memset(ip, '0', INET6_ADDRSTRLEN);
    // First try to convert from a hostname string and set the address
    if (!GetHostIP(target)) {
        exit(1);
    }

    if (useIPv4)
    {
        // Set the TTL value
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0)
        {
            fprintf(stderr, "Setting socket TTL options failed\n");
        }

    }
    else {

        if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &ttl, sizeof(ttl)) != 0)
        {
            fprintf(stderr, "Setting socket hoplimit options failed\n");
        }

        int offset = 2;
        if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_CHECKSUM, &offset, sizeof(offset)) != 0)
        {
            fprintf(stderr, "Setting socket checksum options failed\n");
        }
    }

    // Set the timeout value for receives
    struct timeval timeout;

    unsigned int timeout_seconds = timeoutval / 1000;
    unsigned int timeout_microseconds = (timeoutval - (timeout_seconds * 1000)) * 1000;

    timeout.tv_sec = timeout_seconds; // 3 seconds
    timeout.tv_usec = timeout_microseconds;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                   (const char *)&timeout, sizeof(timeout)) != 0)
    {
        fprintf(stderr, "Setting socket timeout options failed\n");
        exit(1);
    }
}

void PingSocket::pingForever(long int count, long int interval) const {
    uint64_t start, end;
    unsigned long packetsSent = 0;
    unsigned long packetsRecv = 0;
    uint64_t avgRtt = 0;
    uint64_t minRtt = 0;
    uint64_t maxRtt = 0;
    bool packetLost = false;
    struct sockaddr_in stubAddr4; // Store the return address here. It will not be used
    struct sockaddr_in6 stubAddr6; // Store the return address here. It will not be used

    struct sockaddr *pingTargetAddr;
    struct sockaddr *pingRecvAddr;
    size_t addrlen;
    socklen_t recvaddrlen;
    struct echopacket pingPacket;
    u_int16_t buffer[5];
    if (useIPv4) {
        recvaddrlen = sizeof(stubAddr4);
        pingRecvAddr = (struct sockaddr *)&stubAddr4;
        pingPacket.type_and_code = 8;
        pingTargetAddr = (struct sockaddr *)&address;
        addrlen = sizeof(address);
    }
    else {
        recvaddrlen = sizeof(stubAddr6);
        pingRecvAddr = (struct sockaddr *)&stubAddr6;
        pingPacket.type_and_code = 128;
        pingTargetAddr = (struct sockaddr *)&address6;
        addrlen = sizeof(address6);
    }

    unsigned short int seqnum = 1;
    int status;
    bool pingForever = true;

    // Check if ping forever
    if (count >= 0) {
        pingForever = false;
    }

    while (pingForever || count > 0) {
        fprintf(stdout,"Sending ping to %s\n", ip);
        pingPacket.id = seqnum;
        pingPacket.seqnum = seqnum;
        pingPacket.message = seqnum;
        pingPacket.checksum = 0;
        pingPacket.checksum = checksum(pingPacket);
        start = getCurrentTime();

        status = sendto(sockfd, &pingPacket, sizeof(pingPacket), 0, pingTargetAddr, addrlen);
        if (status < 0)
        {
            fprintf(stderr, "Error in sending ping: %i\n", status);
        }
        else
        {
            fprintf(stderr, "Packet sent: %lu bytes, Seq Num: %i, TTL: %li\n", sizeof(pingPacket), seqnum, m_ttl);
            packetsSent += 1;
        }

        if (recvfrom(sockfd, &buffer, sizeof(buffer), 0, pingRecvAddr, &recvaddrlen) <= 0)
        {
            packetLost = true;
        }
        else {
            packetsRecv += 1;
        }
        
        end = getCurrentTime();
        if (!packetLost) {
            uint64_t rtt = (end - start);
            fprintf(stdout, "Received Echo: %lu bytes\nRTT: %" PRIu64 " milliseconds\n", sizeof(buffer), rtt);
            avgRtt += rtt;
            if (packetsRecv == 1) {
                minRtt = rtt;
                maxRtt =rtt;
            }
            else {
                if (rtt > maxRtt) {
                    maxRtt = rtt;
                }
                else if (rtt < minRtt) {
                    minRtt = rtt;
                }
            }
        }
        else {
            fprintf(stderr, "Ping timeout! Packet Lost/Time Exceeded\n");;
        }
    
        packetLost = false;
        ++seqnum;

        fprintf(stdout, "\n");
        if (!pingForever) {
            --count;
        }
        memset(buffer, 0, sizeof(u_int16_t) * 5);
        // Sleep *interval* seconds before pinging again
        sleep(interval);
    }

    // Print the results
    fprintf(stdout, "--- %s ping statistics ---\n", ip);
    fprintf(stdout, "%lu packets transmitted, %lu packets received\n", packetsSent, packetsRecv );
    if (packetsRecv > 0) {
        avgRtt = avgRtt / packetsRecv;
    }
    fprintf(stdout, "RTT min/max/avg = %" PRIu64 "/%" PRIu64 "/%" PRIu64" milliseconds\n", minRtt, maxRtt, avgRtt);
}

bool PingSocket::GetHostIP(char *hostname) {
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if (getaddrinfo(hostname, 0, &hints, &res) != 0) {
        return false;
    }

    if (res == NULL)
    { /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }


    if (res->ai_family == AF_INET) {
        fprintf(stdout, "IPv4\n");
        useIPv4 = true;
        sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sockfd < 0)
        {
           fprintf(stderr, "Could not create socket: %i\n", sockfd);
            exit(1);
        }

        getnameinfo(res->ai_addr, res->ai_addrlen, ip, sizeof(ip), NULL, 0, NI_NUMERICHOST);

        address.sin_family = AF_INET;  
        address.sin_port = htons(0);
        if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0)
        {

            fprintf(stderr, "Invalid hostname/address error\n");
            exit(1);
        }

    }
    else if (res->ai_family == AF_INET6) {  
        fprintf(stdout, "IPv6\n");
        useIPv4 = false;
        sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
        if (sockfd < 0)
        {
            fprintf(stderr, "Could not create socket: %i\n", sockfd);
            exit(1);
        }

        getnameinfo(res->ai_addr, res->ai_addrlen, ip, sizeof(ip), NULL, 0, NI_NUMERICHOST);

        address6.sin6_family = AF_INET6;
        address6.sin6_port = htons(0);
        if (inet_pton(AF_INET6, ip, &address6.sin6_addr) <= 0)
        {

            fprintf(stderr, "Invalid hostname/address error\n");
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res); /* No longer needed */

    fprintf(stdout, "Connected to IP Address: %s\n",ip);

    return true;
}

uint64_t PingSocket::getCurrentTime() const {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + (time.tv_usec / 1000);
}


u_int16_t PingSocket::checksum(struct echopacket packet) const {
    /*
        ICMP Header Checksum. 16 bits. 
        The 16-bit one's complement of the one's complement sum of the ICMP message, 
        starting with the ICMP Type field. When the checksum is computed, the checksum 
        field should first be cleared to 0. When the data packet is transmitted, the 
        checksum is computed and inserted into this field. When the data packet is 
        received, the checksum is again computed and verified against the checksum field. 
        If the two checksums do not match then an error has occurred.
    */
    u_int32_t checksum = 0;
    if (!useIPv4) {
        // Special checksum instructions for ipv6
        /*
            The checksum is the 16-bit one's complement of the one's complement
            sum of the entire ICMPv6 message, starting with the ICMPv6 message
            type field, and prepended with a "pseudo-header" of IPv6 header
            fields, as specified in [IPv6, Section 8.1].  The Next Header value
            used in the pseudo-header is 58.  (The inclusion of a pseudo-header
            in the ICMPv6 checksum is a change from IPv4; see [IPv6] for the
            rationale for this change.)
        */

       
        // 128 bit source address
        // For this, I will convert the IPv4 address to IPv6
        // 0:0:0:0:0:ffff:ipv4

        checksum += 0xffff;

        // STUB: Set source address as 127.0.0.1
        checksum += 0x7f00;
        checksum += 0x0001;

        // 128 bit destination address
        // Split this up into 8 16-bit words
        for(int i = 0; i < 8; ++i) {
            checksum += (u_int16_t)address6.sin6_addr.s6_addr[i * 2];
        }

        // Packet length: 40 bytes header + 10 bytes for packet
        checksum += 50;

        // Next Header is 58
        checksum += 58;
    }

    checksum += packet.type_and_code;
    checksum += packet.checksum;
    checksum += packet.id;
    checksum += packet.seqnum;
    checksum += packet.message;

    checksum = (checksum >> 16) + (checksum & 0xFFFF);
    checksum += (checksum >> 16);
    u_int16_t result = ~checksum;
    
    return result; // one's complement
}