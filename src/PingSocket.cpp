#include "PingSocket.h"

PingSocket::PingSocket(char * target, long int ttl) {

    // First try to convert from a hostname string and set the address
    if (GetHostIPv4(target))
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
        if (sockfd < 0)
        {
            std::cerr << "Could not create socket" << std::endl;
            exit(1);
        }

        // Set the TTL value
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL,
                       &ttl, sizeof(ttl)) != 0)
        {
            std::cerr << "Setting socket TTL options failed" << std::endl;
            exit(1);
        }

        useIPv4 = true;
    }
    else {
        std::cerr << "Not IPv4. Try IPv6" << std::endl;
        // Check if it is IPv6
        if (inet_pton(AF_INET6, target, &address6.sin6_addr) <= 0)
        {
            std::cerr << "Invalid hostname/address error" << std::endl;
            exit(1);
        }
        address6.sin6_family = AF_INET6;
        address6.sin6_port = htons(0); // Port 0

        sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_ICMPV6);
        if (sockfd < 0)
        {   
            std::cerr << "Could not create socket: " << sockfd << std::endl;
            exit(1);
        }

        // // Set the TTL value
        // if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_UNICAST_HOPS,
        //                &ttl, sizeof(ttl)) != 0)
        // {
        //     std::cerr << "Setting socket unicast options failed" << std::endl;
        //     exit(1);
        // }

        // if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
        //                &ttl, sizeof(ttl)) != 0)
        // {
        //     std::cerr << "Setting socket multicast options failed" << std::endl;
        //     exit(1);
        // }

        useIPv4 = false;
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
    ssize_t status;
    struct sockaddr_in stubAddr4; // Store the return address here. It will not be used
    struct sockaddr_in6 stubAddr6; // Store the return address here. It will not be used
    struct echopacket receivedPacket;

    struct sockaddr * pingTargetAddr;
    socklen_t stublen;
    struct echopacket pingPacket;
    if (useIPv4) {
        stublen = sizeof(stubAddr4);
        pingPacket.type = 8;
        pingTargetAddr = (struct sockaddr *)&address;
    }
    else {
        stublen = sizeof(stubAddr6);
        pingPacket.type = 128;
        pingTargetAddr = (struct sockaddr *)&stubAddr6;
    }
    pingPacket.code = 0;

    unsigned short int seqnum = 1;


    while (true) {
        std::cout << "Sending ping" << std::endl;
        pingPacket.id = seqnum;
        pingPacket.seqnum = seqnum;
        pingPacket.checksum = 0;
        pingPacket.checksum = checksum(pingPacket);

        start = getCurrentTime();
        status = sendto(sockfd, &pingPacket, sizeof(pingPacket), 0, pingTargetAddr, (socklen_t)sizeof(pingTargetAddr));
        if (status < 0)
        {
            std::cerr << "Error in sending ping: " << status << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "Packet sent" << std::endl;
        }

        if (recvfrom(sockfd, &receivedPacket, sizeof(receivedPacket), 0, pingTargetAddr, &stublen) <= 0)
        {
            packetLost = true;
        }

        end = getCurrentTime();
        if (!packetLost) {
            std::cout << "Received Echo" << std::endl << "RTT: " << (end - start) << " milliseconds" << std::endl;
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

bool PingSocket::GetHostIPv4(char *hostname) {
    struct hostent *host;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        // No ip for hostname
        return false;
    }
    address.sin_family = host->h_addrtype;
    address.sin_port = htons(0);
    address.sin_addr.s_addr = *(long*)host->h_addr;
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
    if (!useIPv4) {
        // For IPv6, you must prepend the pseudo header

    }
 
    checksum += packet.type;
    checksum += packet.code;
    checksum += packet.checksum;
    checksum += packet.id;
    checksum += packet.seqnum;
    return ~checksum; // one's complement
}