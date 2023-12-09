#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//function to reverse a string
void reverse(char  string[] , int len){

    //reversing a string
    for(int i = 0; i < len/2; i++){
        char temp = string[i];
        string[i] = string[len - i - 1];
        string[len - i - 1] = temp;
    }
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    
        bzero(buffer, BUFFER_SIZE);
        n = recv(newsockfd, buffer, BUFFER_SIZE - 1,0);
        if (n < 0) 
            error("ERROR reading from socket");

        printf("Client: %s", buffer);
        reverse(buffer, strlen(buffer)-1);
        int len= strlen(buffer);
        buffer[len]='\0';
        // printf("Client: %s", buffer);

        // bzero(buffer, BUFFER_SIZE);
        printf("Server: %s", buffer);
        // fgets(buffer, BUFFER_SIZE - 1, stdin);
        n = send(newsockfd, buffer, strlen(buffer),0);
        if (n < 0) 
            error("ERROR writing to socket");
    

    close(newsockfd);
    close(sockfd);
    return 0;
}
