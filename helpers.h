#ifndef _HELPERS_
#define _HELPERS_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "nlohmann/json.hpp"

#define HOST "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"
#define HTTP_TYPE "HTTP/1.1"
#define PORT 8080
#define CMDLEN 30
#define BUFLEN 4096
#define LINELEN 1000
#define BASE 10
#define ERROR_STRING "{\"error"
#define TOO_MANY_REQUESTS_STRING "Too many requests"

using json = nlohmann::json;

void error(const char *msg);

void compute_message(char *message, const char *line);

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

void send_to_server(int sockfd, char *message);

char *receive_from_server(int sockfd);

#endif
