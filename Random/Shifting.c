#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocate memory for an integer using calloc
    int *ptr = (int *)calloc(1, sizeof(int));
    if (ptr == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    // Store a value in the allocated memory
    *ptr = 42;

    // Perform a bitwise left-shift on the data stored at the address
    int *shiftedptr = ptr+1;

    printf("address of Original value: %p\n", ptr);
    printf("address of Shifted value: %p\n", shiftedptr);

    // Free the allocated memory
    free(ptr);

    return 0;
}
