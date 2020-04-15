#include "PingSocket.h"

int main(int argc, char** argv) {
    // Check that the user provided both a ip address and a port
    if (argc < 2) {
        fprintf(stderr, "Insufficient parameters provided\nExpected: ./bin/main $(HOSTNAME/IP ADDRESS)\n");
        exit(1);
    }
    long int ttl = 64;
    if (argc >= 3) {
        // Check that this is a number
        ttl = strtol(argv[2], NULL, 10);
        if (ttl < 0 || ttl > 255) {
            fprintf(stderr, "Invalid TTL specified. TTL will be 64\n");
            ttl = 64;
        }
        else {
            // The user specified a TTL value
            fprintf(stderr, "TTL: %li\n", ttl);
        }   
    }
    
    PingSocket socket = PingSocket(argv[1], ttl);
    fprintf(stderr, "Created socket\n");
    socket.pingForever();
    return 0;
}