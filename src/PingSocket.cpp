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


void PingSocket::pingForever() {
    uint64_t start, end;
    bool packetLost = false;
    struct sockaddr_in stubAddr;  // Store the return address here. It will not be used
    char receivedPacket[PING_PKT_SIZE] = {0};
    socklen_t stubAddrlen = sizeof(stubAddr);

    char* pingPacket  = createPingPacket();
    while (true) {
        std::cout << "Sending ping" << std::endl;

        start = getCurrentTime();
        if (sendto(sockfd, "test", PING_PKT_SIZE, 0, (sockaddr *)&address, (socklen_t)sizeof(address)) < 0)
        {
            std::cerr << "Error in sending ping" << std::endl;
            exit(1);
        }
        if (recvfrom(sockfd, &receivedPacket, sizeof(receivedPacket), 0,
                     (struct sockaddr *)&stubAddr, &stubAddrlen) <= 0)
        {
            std::cerr << "Ping timeout! Packet Lost" << std::endl;
            packetLost = true;
        }
        end = getCurrentTime();
        if (!packetLost) {
            std::cout << "RTT: " << (end - start) << " milliseconds" << std::endl;
        }
        // Sleep 1 second before pinging again
        packetLost = false;
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

uint64_t PingSocket::getCurrentTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

char * PingSocket::createPingPacket() {
    char * packet = (char *)malloc(PING_PKT_SIZE * sizeof(char));

    return packet;
}