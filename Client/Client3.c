#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Connect to localhost
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server on port %d\n", PORT);

    char input[MAX_BUFFER_SIZE];
    while (1) {
        printf("Enter a sentence (or 'GET <filename>'/'PUT <filename>'): ");
        fgets(input, sizeof(input), stdin);

        // Remove newline character from input
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        // Send user input to the server
        send(client_socket, input, strlen(input), 0);

        if (strncmp(input, "GET", 3) == 0) {
            char response[3];
            recv(client_socket, response, 3, 0);

            if (strncmp(response, "FF!", 3) == 0) {
                char filename[MAX_BUFFER_SIZE - 4];
                sscanf(input, "GET %1020s", filename);

                FILE *file = fopen(filename, "wb");
                if (file == NULL) {
                    perror("Error creating file");
                    continue;
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
                printf("File '%s' received and saved.\n", filename);
            }
        } else if (strncmp(input, "PUT", 3) == 0) {

            
        // Extract command and filename
            char command[5], filename[MAX_BUFFER_SIZE - 5];
            sscanf(input, "%4s %1020s", command, filename);

            FILE *file = fopen(filename, "rb");
            if (file == NULL) {
                perror("Error opening file");
                continue;
            }

            // Send "FF!" to indicate file found
            send(client_socket, "FF!", 3, 0);

            char buffer[MAX_BUFFER_SIZE];
            size_t bytes_read;

            while ((bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, file)) > 0) {
                printf("buff:%s:",buffer);
                send(client_socket, buffer, bytes_read, 0);
            }

            fclose(file);
            send(client_socket, "EOF!", 4, 0);


            // char filename[MAX_BUFFER_SIZE - 4];
            // sscanf(input, "PUT %1020s", filename);

            // FILE *file = fopen(filename, "rb");
            // if (file == NULL) {
            //     perror("Error opening file");
            //     continue;
            // }

            // // Send "PUT <filename>" to the server
            // send(client_socket, input, strlen(input), 0);

            // char buffer[MAX_BUFFER_SIZE];
            // size_t bytes_read;

            // while ((bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, file)) > 0) {
            //     send(client_socket, buffer, bytes_read, 0);
            // }

            // fclose(file);
            // send(client_socket, "EOF!", 4, 0);
            // printf("File '%s' sent to the server.\n", filename);
        } else {
            char response[MAX_BUFFER_SIZE];
            recv(client_socket, response, MAX_BUFFER_SIZE, 0);
            printf("Server response: %s\n", response);
        }
    }

    close(client_socket);
    return 0;
}
