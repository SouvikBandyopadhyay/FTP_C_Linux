
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
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

void sendBlankSpacesToClient(int clientSocket, int n) {
    if (n <= 0) {
        return;  // No need to send anything if n is non-positive.
    }

    char spaces[n + 1];  // +1 for null-terminator
    memset(spaces, ' ', n);
    spaces[n] = '\0';  // Null-terminate the string

    send(clientSocket, spaces, n, 0);
}

void sendFileToClient(int clientSocket, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        // If the file doesn't exist, send an error message to the client
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

void recieveFileFromClient(int sockfd,char* filename){
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

// Function designed for chat between client and server.
void func(int connfd, int c_id)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("\nclient%d>>:%s:",c_id, buff);
        
        // Code to Analyse Client Buff
        int wordcount=0;
        char **words=splitSentenceIntoWords(buff,&wordcount);
        if (wordcount>=2){
            removeNewline(words[wordcount-1]);
            if (strcmp(words[0],"get")==0)
            {
                for (int i = 1; i < wordcount; i++)
                {
                    printf("\nget filename is <%s>",words[i]);
                    sendFileToClient(connfd,words[i]);
                }
                continue;
            }
            else if (strcmp(words[0],"put")==0)
            {
                for (int i = 1; i < wordcount; i++)
                {
                    printf("\nput filename is <%s>",words[i]);
                    recieveFileFromClient(connfd,words[i]);
                }
                continue;
            }
            else if (strcmp(words[0],"del")==0)
            {
                for (int i = 1; i < wordcount; i++)
                {
                    printf("\ndel filename is <%s>",words[i]);
                    // Delete funtion
                }
                continue;
            }
        }
        for (int i = 0; i < wordcount; i++)
        {
            printf("\nword no.%d: is :%s:",i,words[i]);
        }



        bzero(buff, MAX);
        printf("\nserver%d>>",c_id); 
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
    }
}
   
// Driver function
int main()
{
    int sockfd, connfd, len;
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
   
    // // Accept the data packet from client and verification
    // connfd = accept(sockfd, (SA*)&cli, &len);
    // if (connfd < 0) {
    //     printf("server accept failed...\n");
    //     exit(0);
    // }
    // else
    //     printf("server accept the client...\n");
    //     printf("port no =%d\n",PORT);

    // // Function for chatting between client and server
    // func(connfd);

    int clientcounter=0;
    while (1)
    {
        
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        clientcounter++;
        pid_t pid=fork();

        if (pid == -1) {
            perror("Error forking");
            close(connfd);
            continue;
        }

        if (pid == 0) {
            // This is the child process, handle the client

            if (connfd < 0) {
                printf("server accept failed...\n");
                exit(0);
            }
            else
                printf("server accept the client...\n");
                printf("port no =%d\n",PORT);

            // Function for chatting between client and server
            func(connfd,clientcounter);
            exit(EXIT_SUCCESS); // Terminate the child process

        } else {
            // This is the parent process, close the client socket
            // close(connfd);
            printf("\nclosed parent\n");
        }
    }

   
    // After chatting close the socket
    close(sockfd);
}

