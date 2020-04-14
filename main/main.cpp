#include <iostream>
#include "PingSocket.h"

int main(int argc, char** argv) {
    // Check that the user provided both a ip address and a port
    if (argc < 2) {
        std::cerr << "Insufficient parameters provided" << std::endl
                  << "Expected: ./bin/main $(HOSTNAME/IP ADDRESS)" << std::endl;
        exit(1);
    }
    long int ttl = 64;
    if (argc >= 3) {
        // Check that this is a number
        ttl = strtol(argv[2], NULL, 10);
        if (ttl <= 0 || ttl > 255) {
            std::cerr << "Invalid TTL specified. TTL will be 64" << std::endl;
            ttl = 64;
        }
        else {
            // The user specified a TTL value
            std::cout << "TTL: " << ttl << std::endl;
        }   
    }
    
    PingSocket socket = PingSocket(argv[1], ttl);
    std::cout << "Created socket" << std::endl;
    socket.pingForever();
    return 0;
}