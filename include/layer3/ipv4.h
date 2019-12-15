#ifndef IPV4_H
#define IPV4_H

#include <stdint.h>
#include <linux/ip.h>

typedef struct iphdr IPv4Header;

inline IPv4Header *IPV4(const uint8_t *packet){
    return (IPv4Header *)packet;
}

static char *getSrcIPv4(const IPv4Header *ip){
    char *srcIP = malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip->saddr, srcIP, INET_ADDRSTRLEN);
    return srcIP;
}

static char *getDstIPv4(const IPv4Header *ip){
    char *srcIP = malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip->daddr, srcIP, INET_ADDRSTRLEN);
    return srcIP;
}

static inline uint16_t getL4ProtoIPv4(const IPv4Header *ip){
    return ip->protocol;
}

static inline uint8_t *getL4HeaderIPv4(const IPv4Header *ip){
    return ((uint8_t *)ip + (ip->ihl * 4));
}

static inline uint32_t getTransportSizeIPv4(const IPv4Header *ip){
    return htons(ip->tot_len) - sizeof(IPv4Header);
}

#endif
