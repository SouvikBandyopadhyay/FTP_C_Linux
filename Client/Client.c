#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8081
#define SA struct sockaddr
#define FILE_NOT_FOUND "\n:File Not Found 404!:\n"


char **splitSentenceIntoWords(const char *sentence, int *wordCount) {
    char *token;
    char *copy, *copyPtr;
    char **words = NULL;
    int count = 0;

    // Make a copy of the sentence to avoid modifying the original
    copy = strdup(sentence);
    if (copy == NULL) {
        perror("Error allocating memory for sentence copy");
        return NULL;
    }

    // Initialize strtok_r's context pointer
    copyPtr = copy;

    // Tokenize the sentence
    token = strtok_r(copyPtr, " ", &copyPtr);

    while (token != NULL) {
        // Allocate memory for the word and copy it
        char *word = strdup(token);
        if (word == NULL) {
            perror("Error allocating memory for word");
            free(copy);
            for (int i = 0; i < count; i++) {
                free(words[i]); // Free previously allocated words
            }
            free(words);
            return NULL;
        }

        // Resize the array of words
        words = (char **)realloc(words, (count + 1) * sizeof(char *));
        if (words == NULL) {
            perror("Error reallocating memory for words");
            free(copy);
            free(word);
            for (int i = 0; i < count; i++) {
                free(words[i]); // Free previously allocated words
            }
            return NULL;
        }

        // Add the word to the array
        words[count] = word;
        count++;

        // Move to the next token
        token = strtok_r(NULL, " ", &copyPtr);
    }

    free(copy);

    // Set the wordCount pointer
    *wordCount = count;

    return words;
}

void removeNewline(char *str) {
    int length = strlen(str);
    
    // Check if the last character is a newline character
    if (length > 0 && str[length - 1] == '\n') {
        // Replace the newline character with a null terminator
        str[length - 1] = '\0';
    }
}

void sendFileToServer(int clientSocket, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        // If the file doesn't exist, send an error message to the client
        printf("\nFNF\n");
        send(clientSocket, FILE_NOT_FOUND, strlen(FILE_NOT_FOUND), 0);
        return;
    }

    char buffer[80];
    size_t bytesRead;

    // Send File found signal
    send(clientSocket, "FILE_FOUND ", strlen("FILE_FOUND "), 0);
    sleep(1);
    // Read and send the file in chunks
    size_t lastsent=0;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer)-1, file)) > 0) {
        buffer[bytesRead]='\0';
        send(clientSocket, buffer, bytesRead, 0);
        printf("\nbytes sent: %zu",bytesRead);
        lastsent=bytesRead;
        bzero(buffer, MAX);
        read(clientSocket, buffer, sizeof(buffer));
        printf("\nRecieved :%s\n",buffer);
        bzero(buffer, MAX);
        sleep(1);
    }

    send(clientSocket, "FILE_END", strlen("FILE_END"), 0);
    read(clientSocket, buffer, sizeof(buffer));
    printf("\n%s\n",buffer);
    bzero(buffer, MAX);
    fclose(file);
}

void recieveFileFromServer(int sockfd,char* filename){
    char filebuff[MAX];
    bzero(filebuff, sizeof(filebuff));
    // check if file found response recieved
    read(sockfd, filebuff, MAX);
    printf("\n:checking file found %s:\n",filebuff);
    int wc1=0;
    char **words1=splitSentenceIntoWords(filebuff,&wc1);
    if (strcmp(words1[0],"FILE_FOUND")==0)
    {
        FILE* file = fopen(filename, "w"); // Open the file in append mode
        if (file == NULL) {
            perror("Error opening the file");
            return;
        }
        if (wc1>1)
        {
            /* code */
            for (size_t i = 1; i < wc1; i++)
            {
                /* code */
                printf("%s ",words1[i]);
            }
            
        }
        
        while (1)
        {
            bzero(filebuff, sizeof(filebuff));
            read(sockfd, filebuff, MAX);
            send(sockfd, "ack", sizeof("ack"), 0);
            printf("\n:file buff %s:\n",filebuff);
            // check if File End response recieved then close
            if (strcmp(filebuff,"FILE_END")==0)
            {
                break;
            }
            // else open a file with file name words i
            else{
            // write to file
                fprintf(file, "%s", filebuff);
            }
        }
        fclose(file);
    }
    
}
    
int analyseprompt(int sockfd,char *buff){
    int wordcount=0;
    char **words=splitSentenceIntoWords(buff,&wordcount);
    if (wordcount<2)
    {
        return -1;
    }
    removeNewline(words[wordcount-1]);
    // check is 1st word is get
    if(strcmp(words[0],"get")==0){
    // for each words 1+
        for (int i = 1; i < wordcount; i++)
        {
            printf("\nFilename :%s:",words[i]);
            recieveFileFromServer(sockfd,words[i]);
        }
    }
    else if (strcmp(words[0],"put")==0)
    {
        for (size_t i = 1; i < wordcount; i++)
        {
            printf("\nFilename :%s:",words[i]);
            sendFileToServer(sockfd,words[i]);
        }
        
    }
    return 0;
}

void func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("\nclient>>");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        if(analyseprompt(sockfd,buff)==0){
            continue;
        }
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("\nFrom Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}
 
int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
 
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
 
    // function for chat
    func(sockfd);
 
    // close the socket
    close(sockfd);
}
