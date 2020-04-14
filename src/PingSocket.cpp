#include "PingSocket.h"

PingSocket::PingSocket(char * target, long int ttl) {
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        std::cerr << "Could not create socket" << std::endl;
        exit(1);
    }

    // First try to convert from a hostname string and set the address
    if (!dnsGetHostIp(target, &address))
    {
        // If target is not a hostname string, check if it is an IP address
        if (inet_pton(AF_INET, target, &address.sin_addr) <= 0)
        {

            std::cerr << "Invalid hostname/address error" << std::endl;
            exit(1);
        }
        address.sin_family = AF_INET;
        address.sin_port = htons(0); // Port 0
    }

    // Set the TTL value
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL,
                   &ttl, sizeof(ttl)) != 0)
    {
        std::cerr << "Setting socket TTL options failed" << std::endl; 
        exit(1);
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
    struct sockaddr_in stubAddr;  // Store the return address here. It will not be used
    struct echopacket receivedPacket;
    socklen_t stubAddrlen = sizeof(stubAddr);
    unsigned short int seqnum = 1;
    struct echopacket pingPacket;
    pingPacket.type = 8;
    pingPacket.code = 0;

    while (true) {
        std::cout << "Sending ping" << std::endl;
        pingPacket.id = seqnum;
        pingPacket.seqnum = seqnum;
        pingPacket.checksum = 0;
        pingPacket.checksum = checksum(pingPacket);

        start = getCurrentTime();
        if (sendto(sockfd, &pingPacket, sizeof(pingPacket), 0, (sockaddr *)&address, (socklen_t)sizeof(address)) < 0)
        {
            std::cerr << "Error in sending ping" << std::endl;
            exit(1);
        }
        
        
        if (recvfrom(sockfd, &receivedPacket, sizeof(receivedPacket), 0,
                    (struct sockaddr *)&stubAddr, &stubAddrlen) <= 0)
        {
            packetLost = true;
        }
        
        end = getCurrentTime();
        if (!packetLost) {
            std::cout << "RTT: " << (end - start) << " milliseconds" << std::endl;
        }
        else {
            std::cerr << "Ping timeout! Packet Lost" << std::endl;
        }
    
        packetLost = false;
        ++seqnum;

        // Sleep 1 second before pinging again
        sleep(1);
    }
}

bool PingSocket::dnsGetHostIp(char *hostname, struct sockaddr_in *address) {
    struct hostent *host;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        // No ip for hostname
        return false;
    }
    address->sin_family = host->h_addrtype;
    address->sin_port = htons(0);
    address->sin_addr.s_addr = *(long*)host->h_addr;
    std::cout << "Connecting to IP Address: " << inet_ntoa(* (struct in_addr *) host->h_addr) << std::endl;

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
    u_int16_t checksum = 0;

    checksum += packet.type;
    checksum += packet.code;
    checksum += packet.checksum;
    checksum += packet.id;
    checksum += packet.seqnum;
    
    return ~checksum; // one's complement
}