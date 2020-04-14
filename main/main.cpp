#include <iostream>
#include "PingSocket.h"

int main(int argc, char** argv) {
    // Check that the user provided both a ip address and a port
    if (argc != 2) {
        std::cerr << "Insufficient parameters provided" << std::endl
                  << "Expected: ./bin/main $(HOSTNAME/IP ADDRESS)" << std::endl;
        exit(1);
    }

    PingSocket socket = PingSocket(argv[1]);
    std::cout << "Created socket" << std::endl;
    socket.pingForever();
    return 0;
}