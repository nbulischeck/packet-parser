#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include <linux/tcp.h>

typedef struct tcphdr TCPHeader;

TCPHeader *TCP(const uint8_t *packet){
    return (TCPHeader *)packet;
}

static inline uint16_t getSrcPortTCP(const TCPHeader *tcp){
    return htons(tcp->source);
}

static inline uint16_t getDstPortTCP(const TCPHeader *tcp){
    return htons(tcp->dest);
}

static inline uint32_t getDataLengthTCP(const TCPHeader *tcp, uint32_t packet_size){
    return packet_size - (tcp->doff * 4);
}

static inline uint8_t *getDataPtrTCP(const TCPHeader *tcp){
    return (uint8_t *)tcp + (tcp->doff * 4);
}

#endif
