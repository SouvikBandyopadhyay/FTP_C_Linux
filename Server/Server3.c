#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define MAX_BUFFER_SIZE 1024

void handle_get_request(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Send "FF!" to indicate file found
    send(client_socket, "FF!", 3, 0);

    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
    send(client_socket, "EOF!", 4, 0);
}

void handle_put_request(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error creating file");
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_received;

    while (1) {
        bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_received <= 0 || strncmp(buffer, "EOF!", 4) == 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept a connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        char request[MAX_BUFFER_SIZE];
        recv(client_socket, request, MAX_BUFFER_SIZE, 0);

        // Extract command and filename
        char command[5], filename[MAX_BUFFER_SIZE - 5];
        sscanf(request, "%4s %1020s", command, filename);

        if (strncmp(command, "GET", 3) == 0) {
            handle_get_request(client_socket, filename);
        } else if (strncmp(command, "PUT", 3) == 0) {
            // handle_put_request(client_socket, filename);

            char responseff[3];
            recv(client_socket, responseff, 3, 0);

            if (strncmp(responseff, "FF!", 3) == 0) {
                // char filename[MAX_BUFFER_SIZE - 4];
                // sscanf(response, "PUT %1020s", filename);

                FILE *file = fopen(filename, "wb");
                if (file == NULL) {
                    printf(":%s:",filename);
                    perror("Error creating file");
                    continue;
                }

                char buffer[MAX_BUFFER_SIZE];
                size_t bytes_received;

                while (1) {
                    bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
                    printf("buff:%s:",buffer);
                    if (bytes_received <= 0 || strncmp(buffer, "EOF!", 4) == 0) {
                        break;
                    }
                    fwrite(buffer, 1, bytes_received, file);
                }

                fclose(file);
                printf("File '%s' received and saved.\n", filename);
            }

        } else {
            printf("Invalid command: %s\n", command);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
