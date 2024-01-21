#include <stdio.h>

int main() {
    const char *filename = "Server.c"; // Replace with the actual file name

    FILE *file = fopen(filename, "r"); // Open the file in read mode

    if (file == NULL) {
        perror("Error opening file");
        return 1; // Return an error code to indicate failure
    }

    // File is successfully opened, you can perform operations on it here.
    printf("done");
    // Don't forget to close the file when you're done with it.
    fclose(file);

    return 0; // Return 0 to indicate success
}
