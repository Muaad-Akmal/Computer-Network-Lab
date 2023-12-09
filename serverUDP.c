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

    int sockfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    bzero(buffer, BUFFER_SIZE);
    clilen = sizeof(cli_addr);
    n = recvfrom(sockfd, buffer, BUFFER_SIZE,0, (struct sockaddr *) &cli_addr, &clilen);
    printf("Client: %s", buffer);
    if(n<0)
      error("ERROR recieving");


    reverse(buffer, strlen(buffer)-1);
    int len= strlen(buffer);
    buffer[n]='\0';
    // printf("Client: %s", buffer);
    // bzero(buffer, BUFFER_SIZE);
    printf("Server: %s", buffer);
    // fgets(buffer, BUFFER_SIZE - 1, stdin);
    clilen = sizeof(cli_addr);
    n = sendto(sockfd, buffer, strlen(buffer),0, (struct sockaddr *) &cli_addr, clilen);

    if(n<0)
      error("ERROR sending");

    close(sockfd);
    return 0;
}


// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>

// int main() {
//     int sockfd;
//     struct sockaddr_in server_addr, client_addr;
//     char buffer[1024];
//     int n;

//     // create socket
//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0) {
//         perror("Error creating socket");
//         exit(1);
//     }

//     // set server address
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     server_addr.sin_port = htons(5555);

//     // bind the socket to the server address
//     if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
//         perror("Error binding socket");
//         exit(1);
//     }

//     // receive message from client
//     socklen_t client_addr_len = sizeof(client_addr);
//     n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
//     if (n < 0) {
//         perror("Error receiving message from client");
//         exit(1);
//     }
//     buffer[n] = '\0';
//     printf("Received message from client: %s\n", buffer);

//     // reverse the string
//     for (int i = 0, j = n - 1; i < j; i++, j--) {
//         char temp = buffer[i];
//         buffer[i] = buffer[j];
//         buffer[j] = temp;
//     }

//     // send reversed message to client
//     n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_addr_len);
//     if (n < 0) {
//         perror("Error sending reversed message to client");
//         exit(1);
//     }

//     // close the socket
//     close(sockfd);

//     return 0;
// }
