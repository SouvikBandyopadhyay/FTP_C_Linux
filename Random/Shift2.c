#include <stdio.h>
#include <stdlib.h>

int main() {
    unsigned char *bytePtr = (unsigned char *)calloc(1, sizeof(unsigned char));

    if (bytePtr == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    // Set the 3rd bit (index 2) to 1
    *bytePtr |= (1 << 7);

    // Check if the 3rd bit is set
    if (*bytePtr & (1 << 7)) {
        printf("The 3rd bit is set (1)\n");
        printf("The %u\n",*bytePtr);
        printf("sizy of char %ld\n",sizeof(unsigned char)); 
        // char is 1 Byte = 8 bit
    } else {
        printf("The 3rd bit is not set (0)\n");
    }

    // Free the allocated memory
    free(bytePtr);

    return 0;
}
