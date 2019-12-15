#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>
#include <linux/icmp.h>

typedef struct icmphdr ICMPHeader;

ICMPHeader *ICMP(const uint8_t *packet){
    return (ICMPHeader *)packet;
}

static inline uint8_t getControlTypeICMP(const ICMPHeader *icmp){
    return icmp->type;
}

static inline uint8_t getControlCodeICMP(const ICMPHeader *icmp){
    return icmp->code;
}

static inline uint32_t getDataLengthICMP(const ICMPHeader *icmp, uint32_t packet_size){
    return packet_size - sizeof(*icmp);
}

static inline uint8_t *getDataPtrICMP(const ICMPHeader *icmp){
    return (uint8_t *)&icmp->un;
}

#endif
