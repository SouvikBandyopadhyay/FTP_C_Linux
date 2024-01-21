#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 1024
#define PORT 8081
#define SA struct sockaddr

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

void analysePUTprompt(int sockfd,char *buff){
    // check if 1st word is PUT
    // search for the files in words 1+
    // start a loop send the file
}

void analyseprompt(int sockfd,char *buff){
    char filebuff[MAX];
    int wordcount=0;
    char **words=splitSentenceIntoWords(buff,&wordcount);
    // check is 1st word is get
    if(strcmp(words[0],"GET")==0){
    // for each words 1+
    for (int i = 1; i < wordcount; i++)
    {
        bzero(filebuff, sizeof(filebuff));
        // check if file found response recieved
        read(sockfd, filebuff, sizeof(filebuff));
        printf("\n:%s:\n",filebuff);
        if (strcmp(filebuff,"FILE_FOUND")==0)
        {
            while (1)
            {
                bzero(filebuff, sizeof(filebuff));
                read(sockfd, filebuff, sizeof(filebuff));
                printf("\n:%s:\n",filebuff);
                // check if File End response recieved then close
                if (strcmp(filebuff,"FILE_END")==0)
                {
                    break;
                }
                
                // else open a file with file name words i
                // write to file
            }
            
        }
        
    }
}
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
        // analyseprompt(sockfd,buff);
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        // Code to analyse Server Response
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






#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8083
#define SA struct sockaddr


void removeNewline(char *str) {
    int length = strlen(str);
    
    // Check if the last character is a newline character
    if (length > 0 && str[length - 1] == '\n') {
        // Replace the newline character with a null terminator
        str[length - 1] = '\0';
    }
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
        printf("\nSending to server :%s:",buff);
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        // removeNewline(buff);
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
