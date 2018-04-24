#ifndef NETQ_H
#define NETQ_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


struct nq {
    int sz;
    int cap;
    int sock;
    char* begin;
    char* end;
    char* data;
};

struct nq* nq_init(int sockfd, int size);
void nq_free(struct nq* queue);
void nq_pull(struct nq* queue);
int nq_pop(struct nq* queue, char* dest, int size);
int nq_pop_until(struct nq* queue, char* dest, int size, int c);


#endif


