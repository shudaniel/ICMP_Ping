# cloudflare2020_application

### To compile:
    `make`

### To run:
    `sudo ./bin/main $(HOSTNAME/IP ADDRESS) [-c count] [-i interval] [-t ttl]` 
    HOSTNAME/IP ADDRESS is the command line argument for the server that you are trying to ping.

    EXAMPLE: Both 'sudo ./bin/main www.google.com' and 'sudo ./bin/main 172.217.14.68'
    
    Each ping request default has a 3 second timeout. One ping is sent every 1 seconds.

### Flags:
    `   
    [-c count]  Send exactly *count* ECHO_REQUEST packets. If not specified, ping will send forever

    [-t ttl] Set the Time to Live

    [-i interval]  Set the interval between packets in seconds
    `

### To clean:
    `make clean`

### IPv6 Support
    IPv6 address can be read in, but through my testing, only ::1 actually will work.
    All other IPv6 will be unable to send through the socket. 
    I suspect that the error comes from computing the checksum, but I am unsure. 
    The checksum for IPv6 should be computed automatically from the documentation I saw.
    I tried to calculate it myself but was unsuccessful.
