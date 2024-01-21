#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1" // Server IP address
#define SERVER_PORT 8080      // Server port number
#define MAXBUFF 1024
#define TERMINAL "BYE\n"
#define MAX_WORDS 100


void getInput(char *sentence, int maxLength) {
    fgets(sentence, maxLength, stdin);
}


char** words(char* sentenceOriginal){

    int count=0;
    char* sentence = strdup(sentenceOriginal);
    char** wordsptr = malloc(MAX_WORDS * sizeof(char*));
    wordsptr[0]=sentence;
    int i=0;
    int len=strlen(sentence);
    for(i=0;i<len-1;i++){
        if(sentence[i]==' '){
            if(sentence[i+1]!=' '){
                count+=1;
                wordsptr[count]=&sentence[i+1];
            }
            sentence[i]='\0';
        }
    }
    count+=1;
    wordsptr[count]=NULL;
    return wordsptr;
}


int createFile(const char *filename) {
    FILE *file = fopen(filename, "w"); // Open the file in write mode ("w")
    if (file == NULL) {
        printf("Unable to create file");
        return -1; // Error creating the file
    }

    fclose(file);
    return 0; // Success
}


int appendBytesToFile(const char *filename, const char *data, size_t n) {
    FILE *file = fopen(filename, "ab"); // Open the file in binary append mode
    if (file == NULL) {
        printf("Unable to open file");
        return -1; // Error opening the file
    }

    size_t bytesWritten = fwrite(data, 1, n, file);
    if (bytesWritten != n) {
        printf("Error writing data to file");
        fclose(file);
        return -1; // Error writing data
    }

    fclose(file);
    return 0; // Success
}


int main() {
    int client_fd;
    struct sockaddr_in server_address;

    // Create a socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    while (1)
    {
        char inputbuff[MAXBUFF];
        char buff[MAXBUFF];
        int valread=read(client_fd,inputbuff,MAXBUFF);
        printf("%s>",inputbuff);
        getInput(buff,MAXBUFF);
        printf("%s,%d",buff,strcmp(buff,TERMINAL));
        send(client_fd,buff,strlen(buff)-1,0);
        if(strcmp(buff,TERMINAL)==0){
            close(client_fd);
            return 0;
        }
        else
        {
            char** wordarray=words(buff);
            if (strcmp(wordarray[0],"GET")==0)
            {
                char *readbuff;
                valread=read(client_fd,readbuff,MAXBUFF);
                if(strcmp(readbuff,"!ERROR")==0){

                }
                else{
                    if(createFile(wordarray[1])==0){
                        while (1)
                        {
                            char filebuff[MAXBUFF];
                            valread=read(client_fd,filebuff,MAXBUFF);
                            printf("File Found :%s\n",filebuff);
                            if(strcmp(filebuff,"EOF!")==0){
                                break;
                            }
                            appendBytesToFile(wordarray[1], filebuff,strlen(filebuff));
                        }
                    }
                }
            }
            
        }
        
    }

    // Perform data exchange with the server here
    // You can use read and write functions to send and receive data

    // Close the client socket when done
    close(client_fd);

    return 0;
}
