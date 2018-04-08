#ifndef HTTP_H
#define HTTP_H


#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


struct http_chunk {
    int sz;
    char *data;
    struct http_chunk *next;

};

#define HTTP_BUFSIZE 1024

char* fetch(const char*);

// helpers
void http_strip_response_header(int sockfd);
void http_decode(int sockfd, char** dest);
void http_send_req(int sockfd, const char* host);

void print_escape(const char* s);


#endif


