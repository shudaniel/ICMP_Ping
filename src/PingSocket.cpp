#include "PingSocket.h"

PingSocket::PingSocket(char * target, long int ttl) {

    // First try to convert from a hostname string and set the address
    if (!GetHostIP(target)) {
        exit(1);
    }

    if (useIPv4)
    {
        // Set the TTL value
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL,
                       &ttl, sizeof(ttl)) != 0)
        {
            std::cerr << "Setting socket TTL options failed" << std::endl;
            exit(1);
        }

    }
    else {
        // Set the TTL value
        if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_HOPLIMIT,
                       &ttl, sizeof(ttl)) != 0)
        {
            std::cerr << "Setting socket hoplimit options failed" << std::endl;
            exit(1);
        }

        // Kernel will calculate checksum
        // This is only supported on SOCK_RAW
        // int offset = 2;
        // int ret = setsockopt(sockfd, IPPROTO_IPV6, IPV6_CHECKSUM, &offset, sizeof(offset));
        // if (ret < 0)
        // {
        //     std::cerr << "Could not set checksum: " << ret << std::endl;
        //     exit(1);
        // }
        

    }

    // Set the timeout value for receives
    struct timeval timeout;
    timeout.tv_sec = 2; // 2 seconds
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                   (const char *)&timeout, sizeof(timeout)) != 0)
    {
        std::cerr << "Setting socket timeout options failed" << std::endl;
        exit(1);
    }
}


void PingSocket::pingForever() const {
    uint64_t start, end;
    bool packetLost = false;
    struct sockaddr_in stubAddr4; // Store the return address here. It will not be used
    struct sockaddr_in6 stubAddr6; // Store the return address here. It will not be used
    struct echopacket receivedPacket;

    struct sockaddr *pingTargetAddr;
    socklen_t stublen;
    struct echopacket pingPacket;
    if (useIPv4) {
        stublen = sizeof(stubAddr4);
        pingPacket.type_and_code = 8;
        pingTargetAddr = (struct sockaddr *)&address;
    }
    else {
        stublen = sizeof(stubAddr6);
        pingPacket.type_and_code = 128;
        pingTargetAddr = (struct sockaddr *)&address6;
    }

    unsigned short int seqnum = 1;


    while (true) {
        std::cout << "Sending ping" << std::endl;
        pingPacket.id = seqnum;
        pingPacket.seqnum = seqnum;
        pingPacket.checksum = 0;
        pingPacket.checksum = checksum(pingPacket);

        start = getCurrentTime();

        if (!sendPing(&pingPacket, &receivedPacket, pingTargetAddr, stublen, true)) {
            packetLost = true;
        }
        
        end = getCurrentTime();
        if (!packetLost) {
            std::cout << "Received Echo: " << sizeof(receivedPacket) << " bytes" << std::endl << "RTT: " << (end - start) << " milliseconds" << std::endl;
        }
        else {
            std::cerr << "Ping timeout! Packet Lost/Time Exceeded" << std::endl;
        }
    
        packetLost = false;
        ++seqnum;

        std::cout << std::endl;
        // Sleep 1 second before pinging again
        sleep(1);
    }
}

bool PingSocket::GetHostIP(char *hostname) {
    struct addrinfo hints;
    struct addrinfo *res;
    char ip[INET6_ADDRSTRLEN] = {0};

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
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
        std::cout << "IPv4" << std::endl;
        useIPv4 = true;
        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
        if (sockfd < 0)
        {
            std::cerr << "Could not create socket: " << sockfd << std::endl;
            exit(1);
        }

        getnameinfo(res->ai_addr, res->ai_addrlen, ip, sizeof(ip), NULL, 0, NI_NUMERICHOST);

        address.sin_family = AF_INET;  
        address.sin_port = htons(0);
        if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0)
        {

            std::cerr << "Invalid hostname/address error" << std::endl;
            exit(1);
        }

    }
    else {  
        std::cout << "IPv6" << std::endl;
        useIPv4 = false;
        sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_ICMPV6);
        if (sockfd < 0)
        {
            std::cerr << "Could not create socket: " << sockfd << std::endl;
            exit(1);
        }

        getnameinfo(res->ai_addr, res->ai_addrlen, ip, sizeof(ip), NULL, 0, NI_NUMERICHOST);

        address6.sin6_family = AF_INET6;
        address6.sin6_port = htons(0);
        if (inet_pton(AF_INET6, ip, &address6.sin6_addr) <= 0)
        {

            std::cerr << "Invalid hostname/address error" << std::endl;
            exit(1);
        }
    }
    freeaddrinfo(res); /* No longer needed */

    std::cout << "Connecting to IP Address: " << ip << std::endl;

    return true;
}

uint64_t PingSocket::getCurrentTime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
    if (!useIPv4) {
        // Special checksum instructions for ipv6
        
        return 0;
    }
    u_int16_t checksum = 0;

    checksum += packet.type_and_code;
    checksum += packet.checksum;
    checksum += packet.id;
    checksum += packet.seqnum;

    return ~checksum; // one's complement
}

bool PingSocket::sendPing(struct echopacket *pingPacket, struct echopacket *receivedPacket, sockaddr *pingTargetAddr, socklen_t len, bool printOutput) const
{
    int status = sendto(sockfd, pingPacket, sizeof(pingPacket), 0, pingTargetAddr, (socklen_t)sizeof(pingTargetAddr));
    if (status < 0)
    {
        if (printOutput) {
            std::cerr << "Error in sending ping: " << status << std::endl;
        }
        return false;
    }
    else
    {
        if (printOutput) {
            std::cout << "Packet sent: " << sizeof(pingPacket) << " bytes" << std::endl;
        }
    }

    if (recvfrom(sockfd, receivedPacket, sizeof(receivedPacket), 0, pingTargetAddr, &len) <= 0)
    {
        return false;
    }
    return true;
}