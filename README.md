# cloudflare2020_application

### To compile:
    `make`

### To run:
    Administrator priviledges are required.
    `sudo ./bin/main $(HOSTNAME/IP ADDRESS) [-c count] [-i interval] [-t ttl] [-W timeout]` 
    HOSTNAME/IP ADDRESS is the command line argument for the server that you are trying to ping.

    EXAMPLE: Both 'sudo ./bin/main www.google.com' and 'sudo ./bin/main 172.217.14.68'
    
    Each ping request default has a 2 second timeout, and one ping is sent every second.

### Flags:
    `   
    Flags may be specified in any order, as long as they all come AFTER the hostname/IP address.  
    Ensure that there is always a space between the flag and its value.

    [-c count]  Send exactly *count* ECHO_REQUEST packets. If not specified, ping will send forever

    [-t ttl] Set the Time to Live

    [-i interval]  Set the interval between packets in seconds

    [-W timeout]  Set the time to wait for a response to the ping before giving up in milliseconds
    `

### To clean:
    `make clean`

### IPv6 Support
    IPv6 address can be read in, but through my testing, only ::1 actually will work.
    All other IPv6 will be unable to send through the socket. 
    I suspect that the error comes from computing the checksum, but I am unsure. 
    The checksum for IPv6 should be computed automatically from the documentation I saw.
    I tried to calculate it myself but was unsuccessful.
