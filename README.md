# cloudflare2020_application

### To compile:
    `make`

### To run:
    `sudo ./bin/main $(HOSTNAME/IP ADDRESS) $(OPTIONAL TTL)` 
    HOSTNAME/IP ADDRESS is the command line argument for the server that you are trying to ping.

    EXAMPLE: Both 'sudo ./bin/main www.google.com' and 'sudo ./bin/main 172.217.14.68'

    If you specify OPTIONAL TTL, than that is the TTL value that the packets will have. 
    
    Any additional command line arguments afterwards will be ignored.

    Each ping request has a 3 second timeout. One ping is sent every 2 seconds.

### To clean:
    `make clean`

### IPv6 Support
    IPv6 address can be read in, but through my testing, only ::1 actually will work.
    All other IPv6 will be unable to send through the socket. 
    I suspect that the error comes from computing the checksum, but I am unsure. 
    The checksum for IPv6 requires additional fields, and even if I attempted to hardcode
    those values, I was still unable to get it correct.
