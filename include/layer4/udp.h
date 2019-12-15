#ifndef UDP_H
#define UDP_H

#include <stdint.h>
#include <linux/udp.h>

typedef struct udphdr UDPHeader;

UDPHeader *UDP(const uint8_t *packet){
    return (UDPHeader *)packet;
}

static inline uint16_t getSrcPortUDP(const UDPHeader *udp){
    return htons(udp->source);
}

static inline uint16_t getDstPortUDP(const UDPHeader *udp){
    return htons(udp->dest);
}

static inline uint32_t getDataLengthUDP(const UDPHeader *udp, __attribute__ ((unused)) uint32_t packet_size){
    return udp->len - sizeof(UDPHeader);
}

static inline uint8_t *getDataPtrUDP(const UDPHeader *udp){
    return (uint8_t *)udp + sizeof(UDPHeader);
}

#endif
