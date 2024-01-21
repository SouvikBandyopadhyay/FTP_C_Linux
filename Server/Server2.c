
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <dirent.h>

#define TERMINAL "BYE"
#define MAX_WORDS 100
#define PORT 8080 // Port number for the server
#define MAX_FILES 100 // Maximum number of files in the directory
#define MAX_FILENAME_LENGTH 256 // Maximum length of a filename
#define MAX_FILE_CONTENT_LENGTH 10000 // Maximum length of file content

// Structure to store file information
struct FileInfo {
    char filename[MAX_FILENAME_LENGTH];
    char content[MAX_FILE_CONTENT_LENGTH];
};

int listFilesInCurrentDirectory(struct FileInfo *files) {
    DIR *dir;
    struct dirent *entry;
    int fileCount = 0;

    // Open the current working directory
    dir = opendir(".");
    if (dir == NULL) {
        perror("Unable to open directory");
        return -1; // Return an error code
    }

    // Read and store file names and content
    while ((entry = readdir(dir)) != NULL && fileCount < MAX_FILES) {
        // if (entry->d_type == DT_REG) { // Check if it's a regular file
            strncpy(files[fileCount].filename, entry->d_name, sizeof(files[fileCount].filename));
            files[fileCount].filename[sizeof(files[fileCount].filename) - 1] = '\0'; // Ensure null-termination

            // Read file content
            FILE *file = fopen(entry->d_name, "r");
            if (file == NULL) {
                perror("Unable to open file");
                continue; // Skip to the next file if there's an error opening the file
            }

            // Read file content into the FileInfo structure
            size_t bytesRead = fread(files[fileCount].content, 1, sizeof(files[fileCount].content), file);
            files[fileCount].content[bytesRead] = '\0'; // Ensure null-termination
            fclose(file);

            fileCount++;
        // }
    }

    // Close the directory
    closedir(dir);

    return fileCount;
}

int changeCurrentDirectory(const char *path) {
    if (chdir(path) == 0) {
        printf("Changed current working directory to: %s\n", path);
        return 0; // Success
    } else {
        perror("chdir");
        return -1; // Failure
    }
}

void getInput(char *sentence, int maxLength) {
    fgets(sentence, maxLength, stdin);
}

void printwordsfromarr(char* arr[]){
    printf("\nfuntion printing words 1st word %s\n",arr[0]);
    for(int i=0;arr[i]!=NULL;i++){
        printf("elem %d points to %s\n",i,arr[i]);
    }
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

int sendFileThroughSocket(int socket, const char *filename, int n) {
    printf("\nFT Begin\n");
    FILE *file = fopen(filename, "rb"); // Open the file in binary read mode
    if (file == NULL) {
        perror("Unable to open file");
        send(socket, "ERROR!", 6, 0);
        return -1; // Error opening the file
    }

    char buffer[n];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, n, file)) > 0) {
        ssize_t bytesSent = send(socket, buffer, bytesRead, 0);
        if (bytesSent < 0) {
            perror("Error sending data through socket");
            fclose(file);
            return -1; // Error sending data
        }
    }

    fclose(file);
    return 0; // Success
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char* prompt="prompt";
    
    int valread;

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept incoming connections and handle them
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        // Handle the new connection (e.g., read/write data)
        // You can add your server logic here
        while (1)
        {
            printf("new loop");
            char path [1024];
            char buff[1024]={0};
            getcwd(path, 1024);
            printf("%s",path);
            send(new_socket,path,strlen(path),0);
            valread=read(new_socket,buff,1024);
            printf("\ninput : %s : end of input\n",buff);
            if(strcmp(buff,TERMINAL)==0){
                close(new_socket);
                break;
            }
            else{
                char** wordarray=words(buff);
                printf("1stword %s",wordarray[0]);
                if(strcmp(wordarray[0],"ls")==0){
                    struct FileInfo files[MAX_FILES];
                    int fileCount = listFilesInCurrentDirectory(files);

                    if (fileCount > 0) {
                        for (int i = 0; i < fileCount; i++) {
                            printf("%s\n", files[i].filename);
                            // printf("Content:\n%s\n", files[i].content);
                        }
                    } else if (fileCount == 0) {
                        printf("No files found in the current directory.\n");
                    } else {
                        printf("Error listing files in the current directory.\n");
                    }
                }
                else if(strcmp(wordarray[0],"cd")==0){
                    printf("\nin cd %s\n",wordarray[1]);
                    if(changeCurrentDirectory("testdir")==0){
                        getcwd(path, 1024);
                    }
                    else{
                        printf("directory dont exist");
                    }
                    printf(":end of cd\n");
                }
                else if(strcmp(wordarray[0],"GET")==0){
                    if (sendFileThroughSocket(new_socket, wordarray[1], 1024) == 0) {
                        printf("\nFT Success\n");
                        // Successfully sent the file data through the socket.
                        // You can perform additional tasks as needed.
                    } else {
                        // Handle the error if sending the file data fails.
                    }
                }
            }
            printf(":end of loop\n");
        }
        // Close the socket for this client
        close(new_socket);
    }

    return 0;
}
