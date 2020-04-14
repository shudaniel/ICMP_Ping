#include <iostream>
#include "PingSocket.h"

int main(int argc, char** argv) {
    PingSocket socket = PingSocket("127.0.0.1", 8081);
    std::cout << "Created socket" << std::endl;
    socket.close();
    return 0;
}