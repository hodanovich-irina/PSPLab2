#pragma warning( disable : 4996)
#include <stdio.h>
#include <string>
#include <cstring>
#include <sstream>
#include <time.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>

#define MY_PORT 8081

#define PRINTNUSERS if (nclients) printf("%d user onnline\n",nclients);else printf("No User on line\n");

void* MyAccept(void* client_socket);

int count_calls = 0;
int matrix_size;
int matrix_col_now;
int nclients;
int wasclients;
int max_nclients;

int a;
int b;
int threadNumber = 0;
double resSquare = 0;

clock_t start;

int main(int argc, char* argv[]) {

    // config server
    char buff[1024];
    printf("SERVER STARTED ON PORT %d\n", MY_PORT);
    char host[256];
    char* ip;
    char* ip_i;
    struct hostent* host_entry;
    int hostname = gethostname(host, sizeof(host));
    printf("SERVER NAME: %s\n", host);
    host_entry = gethostbyname(host);
    int iter = 0;
    while ((struct in_addr*)host_entry->h_addr_list[iter]) {
        ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[iter++]));
        printf("Server address: %s : %d\n", ip, MY_PORT);
    }

    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MY_PORT);
    local_addr.sin_addr.s_addr = 0;

    if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr))) {
        close(sock);
        perror("bind");
        exit(2);
    }

    if (listen(sock, 0x100)) {
        printf("Error listen\n");
        close(sock);
        return 1;
    }
    // end config server

    printf("Enter count clients: ");
    scanf("%d", &max_nclients);
    printf("Enter A: ");
    scanf("%d", &a);
    printf("Enter B: ");
    scanf("%d", &b);

    printf("Operations per client = %d\n", max_nclients);

    printf("Waiting connections...\n\n");

    // sending tasks for clients
    int client_socket;
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    while ((client_socket = accept(sock, (sockaddr*)&client_addr, &client_addr_size))) {
        if (wasclients >= max_nclients) {
            break;
        }

        nclients++;
        wasclients++;
        printf("New client accepted!\n");
        pthread_t threadId;
        pthread_create(&threadId, NULL, &MyAccept, &client_socket);
        printf("Wait connectings..\n");
    }
    
    std::cout << "\n\nresult square = " << resSquare << "\n";
    
    return 0;
}

void* MyAccept(void* client_socket)
{
    int sock;
    sock = ((int*)client_socket)[0];
    
    double step = (b - a) / max_nclients;
    double x1 = step * (double)threadNumber + a;
    double x2 = step * (1 + (double)threadNumber) + a;
    
    threadNumber++;
    
    std::string sendMessage = std::to_string(x1) + ' ' + std::to_string(x2);
    
    char* buff = new char[100];
    strcpy(buff, sendMessage.c_str());

    send(sock, buff, 100, 0);
    
    char* buffRes = new char[100];
    recv(sock, buffRes, 100, 0);
    
    std::string receiveMessage = "";
    for (int i = 0; i < 100; i++)
    {
    	receiveMessage += buffRes[i];
    }
    
    double square = atof(receiveMessage.c_str());
    resSquare += square;
    
    std::cout << "way = [" << x1 << ", " << x2 << "]\n";
    std::cout << "square = " << square << "\n";
    
    return 0;
}
