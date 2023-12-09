#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

#define BUFFER_SIZE 256

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    int n;

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // inet_pton(AF_INET,argv[1], &serv_addr.sin_addr.s_addr);
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
    // bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

  struct sockaddr_in serveraddr, clientaddr;

  serveraddr.sin_family= AF_INET;
  serveraddr.sin_port= htons(portno);
  inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

  socklen_t servlen;

  
        bzero(buffer, BUFFER_SIZE);
        printf("Client: ");
        fgets(buffer, BUFFER_SIZE - 1, stdin);
        servlen= sizeof(serveraddr);
        n = sendto(sockfd, buffer, strlen(buffer),0, (struct sockaddr *) &serveraddr, servlen );

      if(n<0){
        error("ERROR writing to socket");
      }

        n = recvfrom(sockfd,buffer, 255,0, NULL, NULL );
        if (n < 0) 
            error("ERROR reading from socket");
        // buffer[n]='\0';
        printf("Received message from server: %s", buffer);

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
//     struct sockaddr_in server_addr;
//     char message[1024];
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
//     server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//     server_addr.sin_port = htons(5555);

//     // get message from user
//     printf("Enter message: ");
//     fgets(message, sizeof(message), stdin);

//     // send message to server
//     n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
//     if (n < 0) {
//         perror("Error sending message to server");
//         exit(1);
//     }

//     // receive reversed message from server
//     n = recvfrom(sockfd, message, sizeof(message), 0, NULL, NULL);
//     if (n < 0) {
//         perror("Error receiving reversed message from server");
//         exit(1);
//     }
//     message[n] = '\0';
//     printf("Received reversed message from server: %s\n", message);

//     // close the socket
//     close(sockfd);

//     return 0;
// }
