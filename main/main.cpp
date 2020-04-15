#include "PingSocket.h"

int main(int argc, char** argv) {
    // Check that the user provided both a ip address and a port
    if (argc < 2) {
        fprintf(stderr, "Insufficient parameters provided\nExpected: ./bin/main $(HOSTNAME/IP ADDRESS)\n");
        exit(1);
    }
    long int count = -1;
    long int ttl = DEFAULT_TTL;
    long int interval = DEFAULT_INTERVAL;
    if (argc >= 3) {
        for(int i = 2; i < argc; ++i) {

            if(strcmp(argv[i], "-t") == 0){
                if (i + 1 < argc) {
                    // Check that this is a number
                    ttl = strtol(argv[i + 1], NULL, 10);
                    if (ttl <= 0 || ttl > 255)
                    {
                        fprintf(stderr, "Invalid TTL specified. TTL will be 64\n");
                        ttl = DEFAULT_TTL;
                    }
                    else
                    {
                        fprintf(stderr, "TTL: %li\n", ttl);
                        ++i;
                    }

                }
            }
            if(strcmp(argv[i], "-c") == 0){
                if (i + 1 < argc)
                {
                    // Check that this is a number
                    count = strtol(argv[i + 1], NULL, 10);
                    if (count <= 0)
                    {
                        fprintf(stderr, "Invalid count specified\n");
                        count = -1;
                    }
                    else
                    {
                        fprintf(stderr, "Count: %li\n", count);
                        ++i;
                    }
                }
            }
            if(strcmp(argv[i], "-i") == 0){
                if (i + 1 < argc)
                {
                    // Check that this is a number
                    interval = strtol(argv[i + 1], NULL, 10);
                    if (interval <= 0)
                    {
                        fprintf(stderr, "Invalid interval specified. Interval will be 1 second\n");
                        interval = DEFAULT_INTERVAL;
                    }
                    else
                    {
                        fprintf(stderr, "Interal: %li\n", interval);
                        ++i;
                    }
                }
            }

            
        } 
    }
    
    PingSocket socket = PingSocket(argv[1], ttl, interval);
    fprintf(stderr, "Created socket\n");
    socket.pingForever(count);
    return 0;
}