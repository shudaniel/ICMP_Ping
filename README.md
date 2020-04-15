# cloudflare2020_application

### To compile:
    `make`

### To run:
    `sudo ./bin/main $(HOSTNAME/IP ADDRESS) $(OPTIONAL TTL)` 
    HOSTNAME/IP ADDRESS is the command line argument for the server that you are trying to ping.

    If you specify OPTIONAL TTL, than that is the TTL value that the packets will have. 
    
    Any additional command line arguments afterwards will be ignored.

    Each ping request has a 3 second timeout. One ping is sent every 2 seconds.

### To clean:
    `make clean`

