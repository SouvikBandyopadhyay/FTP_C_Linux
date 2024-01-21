
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8083
#define SA struct sockaddr
#define MAX_WORD_LENGTH 100
#define MAX_BUFF 1024

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
   
// Function designed for chat between client and server.
void func(int connfd, int cl_id)
{
    char buff[MAX_BUFF];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
        // read the message from client and copy it in buffer
        read(connfd, buff, 1024);
        // print buffer which contains the client contents
        printf("\nclient%d>>%s::",cl_id, buff);

        // Code to Analyse Client Buff
        int wordcount=0;
        char **words=splitSentenceIntoWords(buff,&wordcount);
        for (int i = 0; i < wordcount; i++)
        {
            printf("\nword no.%d: is :%s:",i,words[i]);
        }
        if (wordcount>=2){
            removeNewline(words[wordcount-1]);
            if (strcmp(words[0],"get")==0)
            {
                for (int i = 1; i < wordcount; i++)
                {
                    printf("get filename is <%s>",words[i]);
                }
            }
            else if (strcmp(words[0],"put")==0)
            {
                for (int i = 1; i < wordcount; i++)
                {
                    printf("put filename is <%s>",words[i]);
                }
            }
            else if (strcmp(words[0],"del")==0)
            {
                for (int i = 1; i < wordcount; i++)
                {
                    printf("del filename is <%s>",words[i]);
                }
            }
            
        }
        

        
        

        printf("\nserver%d>>",cl_id); 
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;
   
        // and send that buffer to client
        write(connfd, buff, sizeof(buff));
   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
        close(connfd);
    }
    close(connfd);
}
   
// Driver function
int main()
{
    int sockfd, client_socket, len;
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
    int clientcounter=0;
    while (1)
    {
        
        // Accept the data packet from client and verification
        client_socket = accept(sockfd, (SA*)&cli, &len);
        clientcounter++;
        pid_t pid=fork();

        if (pid == -1) {
            perror("Error forking");
            close(client_socket);
            continue;
        }

        if (pid == 0) {
            // This is the child process, handle the client

            if (client_socket < 0) {
                printf("server accept failed...\n");
                exit(0);
            }
            else
                printf("server accept the client...\n");
                printf("port no =%d\n",PORT);

            // Function for chatting between client and server
            func(client_socket,clientcounter);
            exit(EXIT_SUCCESS); // Terminate the child process

        } else {
            // This is the parent process, close the client socket
            // close(client_socket);
            printf("\nclosed parent\n");
        }
    }
    
    // After chatting close the socket
    close(sockfd);
}

