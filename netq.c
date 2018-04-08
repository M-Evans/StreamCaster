#include "netq.h"

/* thanks to: https://stackoverflow.com/a/3437484 */
#define min(a, b) \
    ({__typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      _a > _b ? _a : _b;})


struct nq* nq_init(int sock) {
    struct nq *ret = malloc(sizeof(struct nq));
    ret->sz = 0;
    ret->cap = NETQ_SIZE;
    ret->sock = sock;
    ret->begin = ret->end = ret->data = malloc(NETQ_SIZE);
    return ret;
}

//recv(s, rcv+pos, HTTP_MAX_PAGE_SIZE_ALLOWED-pos, 0)
void nq_pull(struct nq* q) {
    int got;
    int left = min(q->data + q->cap - q->end, q->cap - q->sz);
    if ((got = recv(q->sock, q->end, left, 0)) < 0) {
        perror("recv");
        exit(1);
    }
    q->sz += got;
    if (q->sz == q->cap) {
        q->end = q->begin;
    } else if (got == left) {
        q->end = q->data;
    }
}

int nq_pop(struct nq* q, char* dst, int sz) {
    return nq_pop_until(q, dst, sz, -1);
}

int nq_pop_until(struct nq* q, char* dst, int sz, int c) {
    int i;
    unsigned char next;
    for (i = 0; i < sz; ++i) {
        if (q->sz == 0) {
            nq_pull(q);
            if (q->sz == 0) {
                return i;
            }
        }
        dst[i] = next = *(q->begin);
        q->begin++;
        q->sz--;
        if (q->begin == q->data + q->cap) {
            q->begin = q->data;
        }
        if (next == c) {
            return i + 1;
        }
    }
    return i;
}

void nq_free(struct nq* q) {
    free(q->data);
    memset(q, 0, sizeof(struct nq));
    free(q);
}


