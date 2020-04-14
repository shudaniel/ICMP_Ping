#include "PingSocket.h"

PingSocket::PingSocket(char * ipaddr, unsigned int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Could not create socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, ipaddr, &address.sin_addr) <= 0) {
        std::cerr << "Invalid address error" << std::endl;
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        exit(1);
    }
}

void PingSocket::close() {
    ::close(sockfd);
}