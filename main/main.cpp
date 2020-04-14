#include <iostream>
#include "PingSocket.h"

int main(int argc, char** argv) {
    // Check that the user provided both a ip address and a port
    if (argc != 3) {
        std::cerr << "Insufficient parameters provided" << std::endl
            << "Expected: ./bin/main $(IP ADDRESS) $(PORT NUMBER)" << std::endl;
        exit(1);
    }

    PingSocket socket = PingSocket(argv[1], (unsigned int)(*argv[2]));
    std::cout << "Created socket" << std::endl;
    socket.pingForever();
    socket.close();
    return 0;
}