#pragma warning(disable: 4996)
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <omp.h>
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8081

char SERVERADDR[15] = "127.0.1.1";

double function(double x);
double method_simpson(double b, double a);
std::string to_string(char* arr, int size);

double function(double x) 
{
	return pow(x, 3) * sin(x);
}

double method_simpson(double b, double a) 
{
	int n = 2;
	double h = (b - a) / n; // вычисляем шаг - h
	double sum = 0; // сумма, результат вычисления интеграла.
	double x0 = a; // правая граница подотрезка отрезка [a, b]
	double x1 = a + h; // левая граница подотрезка отрезка [a, b]

	for (int i = 0; i < n; i++) // в цикле применяем формулу Симпсона
	{
		sum += function(x0) + 4 * function(x0 + h / 2) + function(x1); 
		x0 += h; 
		x1 += h; 
	}

	return (h / 6) * sum; 
}

int main(int argc, char* argv[]) {
    printf("SERVER IP: %s\n", SERVERADDR);
    char buff[1024];

    printf("CLIENT STARTED\n");

    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket error\n");
        return 1;
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);

    int iResult = connect(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (iResult < 0) {
        close(sock);
        printf("Unable to connect to the server\n");
        return 1;
    }

    recv(sock, buff, 100, 0);
    
    double x1, x2, sq;
    
    std::string message = to_string(buff, 100);
    
    x1 = atof(message.substr(0, message.find(' ')).c_str());
    x2 = atof(message.substr(message.find(' ') + 1, message.length() - message.find(' ') - 1).c_str());
    
    sq = method_simpson(x1, x2);
    
    std::cout << "x1 = " << x1 << ", x2 = " << x2 << "\n"; 
    std::cout << "Square = " << sq << "\n"; 
    
    std::string result = std::to_string(sq);
    send(sock, result.c_str(), 100, 0);

    return 0;
}

std::string to_string(char* arr, int size)
{
	std::string s = "";
	for (int i = 0; i < size; i++)
	{
		s = s + arr[i];
	}
	
	return s;
}
