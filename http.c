#include "http.h"
#include "netq.h"


char* fetch(const char* url) {
    printf("%s\n--------------------\n", url); fflush(stdout);
    // open a socket to url
    int status, s;
    struct addrinfo params;
    struct addrinfo *results, *p;

    memset(&params, 0, sizeof params);
    params.ai_family = AF_UNSPEC;
    params.ai_socktype = SOCK_STREAM;
    params.ai_flags = AI_CANONNAME;

    printf("looking up... "); fflush(stdout);
    if ((status = getaddrinfo(url, "http", &params, &results)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status)); fflush(stdout);
        exit(1);
    }
    printf("complete.\n"); fflush(stdout);
    
    //*
    char ip4[INET_ADDRSTRLEN];
    for (p = results; p != NULL; p = p->ai_next) {
        printf("canonname: '%s'\n", p->ai_canonname); fflush(stdout);
        printf("socktype: '%s'\n", (p->ai_socktype == SOCK_STREAM) ? "SOCK_STREAM" : "SOCK_DGRAM"); fflush(stdout);
        printf("ai_family: '%s'\n", (p->ai_family == AF_INET) ? "AF_INET" : "AF_INET6"); fflush(stdout);
        inet_ntop(AF_INET, &(((struct sockaddr_in*)(p->ai_addr))->sin_addr.s_addr), ip4, INET_ADDRSTRLEN);
        printf("ip: '%s'\n\n", ip4); fflush(stdout);
    }
    // */
    
    //*
    for (p = results; p != NULL; p = p->ai_next) {
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            perror("socket");
            if (p->ai_next == NULL) {
                exit(1);
            }
        }
        break;
    }
    // */ s = 1; s = s;

    if (connect(s, p->ai_addr, p->ai_addrlen) < 0) {
        perror("connect");
        exit(1);
    }
    

    printf("sending... "); fflush(stdout);
    http_send_req(s, url);
    printf("sent.\n"); fflush(stdout);
    printf("got:\n===========================\n"); fflush(stdout);

    char* buf = malloc(HTTP_BUFSIZE);
    struct nq* q = nq_init(s);
    int sz;
    while ((sz = nq_pop_until(q, buf, HTTP_BUFSIZE-1, '\n')) > 0) {
        buf[sz] = 0;
        print_escape(buf);
    }
    free(buf);
    nq_free(q);
    //char* rcv;
    //http_decode(s, &rcv);
    //printf("received? nahhhh...\n"); fflush(stdout);
    //printf("\n%s\n\n", rcv); fflush(stdout);
    printf("\n");

    //free(rcv);
    freeaddrinfo(results);
    close(s);
    return NULL;
}

/*
===================== Sample Data: ===============
HTTP/1.1 200 OK\r\n
Server: nginx/1.10.3 (Ubuntu)\r\n
Date: Fri, 06 Apr 2018 08:56:17 GMT\r\n
Content-Type: text/html\r\n
Transfer-Encoding: chunked\r\n
Connection: close\r\n
Vary: Accept-Encoding\r\n
Expires: Fri, 06 Apr 2018 08:56:27 GMT\r\n
Cache-Control: max-age=10\r\n
X-Proxy-Cache: EXPIRED\r\n
X-Proxy-Cache: EXPIRED\r\n
Access-Control-Allow-Origin: *\r\n
\r\n
ebd\r\n
==================================================
// */

void http_strip_response_header(int s) {
    char* buf = malloc(HTTP_BUFSIZE);
    struct nq* q = nq_init(s);
    int sz;
    while ((sz = nq_pop_until(q, buf, HTTP_BUFSIZE, '\n')) > 2);
    free(buf);
    nq_free(q);
}

/*
    int               sz;
    char     *data;
    struct http_chunk *next;
// */
void http_decode(int s, char** dst) {
    http_strip_response_header(s);

    char* buf = malloc(HTTP_BUFSIZE);
    struct nq* q = nq_init(s);
    int sz;

    struct http_chunk *p, *head = malloc(sizeof(struct http_chunk));
    memset(head, 0, sizeof(struct http_chunk));

    for (p = head; p != NULL; p = p->next) {
        // read next chunk size
        sz = nq_pop_until(q, buf, HTTP_BUFSIZE, '\n');
        buf[sz] = 0;
        if (sscanf(buf, "%x", &(p->sz)) < 1) {
            fprintf(stderr, "ERROR: did not receive chunk size. ("); fflush(stdout);
            print_escape(buf);
            fprintf(stderr, ")\n"); fflush(stdout);
            exit(1);
        }
        printf("size of next chunk: %d\n", p->sz); fflush(stdout);
        if (p->sz == 0) continue;

        // setup current chunk data
        p->data = malloc(p->sz);
        // and next chunk
        p->next = malloc(sizeof(struct http_chunk));
        memset(p->next, 0, sizeof(struct http_chunk));

        // read data
        sz = nq_pop(q, p->data, p->sz);
        if (sz < p->sz) {
            fprintf(stderr, "ERROR: did not receive advertised chunk size.\n"); fflush(stdout);
            exit(1);
        }
    }
    
    *dst = malloc(1024);
}

void http_send_req(int s, const char* host) {
    const char* http_get = "GET / HTTP/1.1\r\n";
    const char* http_host = "Host: ";
    const char* http_connection = "Connection: close\r\n";
    send(s, http_get, strlen(http_get), 0);
    send(s, http_host, strlen(http_host), 0);
    send(s, host, strlen(host), 0);
    send(s, "\r\n", strlen("\r\n"), 0);
    send(s, http_connection, strlen(http_connection), 0);
    send(s, "\r\n", strlen("\r\n"), 0);
}

void print_escape(const char* s) {
    int sz = strlen(s);
    int i;
    for (i = 0; i < sz; ++i) {
        switch(s[i]) {
            case '\a':
                printf("\\a"); fflush(stdout);
                break;
            case '\b':
                printf("\\b"); fflush(stdout);
                break;
            case '\f':
                printf("\\f"); fflush(stdout);
                break;
            case '\n':
                printf("\\n"); fflush(stdout);
                break;
            case '\r':
                printf("\\r"); fflush(stdout);
                break;
            case '\t':
                printf("\\t"); fflush(stdout);
                break;
            case '\v':
                printf("\\v"); fflush(stdout);
                break;
            default:
                printf("%c", s[i]); fflush(stdout);
        }
    }
}


