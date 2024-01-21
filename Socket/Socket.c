#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main() {
    printf("Starting Socket \n");
    struct addrinfo hints, *result;
    int status;

    // Initialize the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    // Get address information
    if ((status = getaddrinfo("www.google.com", "80", &hints, &result)) != 0) {
        printf("Stage 1");
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }
    else{
        printf("Stage 2 result: %d\n",(result->ai_socktype));
    }

    // You can now use 'result' to access address information

    // Clean up the result
    freeaddrinfo(result);

    return 0;
}
