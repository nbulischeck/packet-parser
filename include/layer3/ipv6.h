#ifndef IPV6_H
#define IPV6_H

#include <stdint.h>
#include <linux/ipv6.h>

#define NEXTHDR_HOP         0	/* Hop-by-hop option header. */
#define NEXTHDR_TCP         6	/* TCP segment. */
#define NEXTHDR_UDP         17	/* UDP message. */
#define NEXTHDR_IPV6        41	/* IPv6 in IPv6 */
#define NEXTHDR_ROUTING     43	/* Routing header. */
#define NEXTHDR_FRAGMENT    44	/* Fragmentation/reassembly header. */
#define NEXTHDR_ESP         50	/* Encapsulating security payload. */
#define NEXTHDR_AUTH        51	/* Authentication header. */
#define NEXTHDR_ICMP        58	/* ICMP for IPv6. */
#define NEXTHDR_NONE        59	/* No next header */
#define NEXTHDR_DEST        60	/* Destination options header. */
#define NEXTHDR_MOBILITY    135	/* Mobility header. */

#define NEXTHDR_MAX         255

typedef struct ipv6hdr IPv6Header;

inline int ipv6_ext_hdr(const uint8_t nexthdr){
	/*
	 * find out if nexthdr is an extension header or a protocol
	 */
	return  (nexthdr == NEXTHDR_HOP)        ||
            (nexthdr == NEXTHDR_ROUTING)    ||
            (nexthdr == NEXTHDR_FRAGMENT)   ||
            (nexthdr == NEXTHDR_AUTH)       ||
            (nexthdr == NEXTHDR_NONE)       ||
            (nexthdr == NEXTHDR_DEST);
}

inline IPv6Header *IPV6(const uint8_t *packet){
    return (IPv6Header *)packet;
}

static char *getSrcIPv6(const IPv6Header *ip){
    char *srcIP = malloc(INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6, &ip->saddr, srcIP, INET6_ADDRSTRLEN);
    return srcIP;
}

static char *getDstIPv6(const IPv6Header *ip){
    char *srcIP = malloc(INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6, &ip->daddr, srcIP, INET6_ADDRSTRLEN);
    return srcIP;
}

static uint16_t getL4ProtoIPv6(const IPv6Header *ip){
    uint8_t nexthdr = ip->nexthdr;
    uint16_t frag_off = sizeof(IPv6Header);

    while (ipv6_ext_hdr(nexthdr)){
        struct ipv6_opt_hdr *hp;
        int hdrlen;

        if (nexthdr == NEXTHDR_NONE)
            return NEXTHDR_NONE;

        hp = (struct ipv6_opt_hdr *)((uint8_t *)ip + frag_off);
        if (hp == NULL)
            return -1;

        if (nexthdr == NEXTHDR_FRAGMENT){
            uint16_t *fp;

            fp = (uint16_t *)((uint8_t *)ip + frag_off);
            if (fp == NULL)
                return -1;

            if (ntohs(*fp) & ~0x7)
                break;

            hdrlen = 8;
        } else if (nexthdr == NEXTHDR_AUTH){
            hdrlen = (hp->hdrlen+2) << 2;
        } else {
            hdrlen = (hp->hdrlen+1) << 3;
        }

        nexthdr = hp->nexthdr;
        frag_off += hdrlen;
    }

    return nexthdr;
}

static uint8_t *getL4HeaderIPv6(const IPv6Header *ip){
    uint8_t nexthdr = ip->nexthdr;
    uint16_t frag_off = sizeof(IPv6Header);

    while (ipv6_ext_hdr(nexthdr)){
        struct ipv6_opt_hdr *hp;
        int hdrlen;

        if (nexthdr == NEXTHDR_NONE)
            return NULL;

        hp = (struct ipv6_opt_hdr *)((uint8_t *)ip + frag_off);
        if (hp == NULL)
            return NULL;

        if (nexthdr == NEXTHDR_FRAGMENT){
            uint16_t *fp;

            fp = (uint16_t *)((uint8_t *)ip + frag_off);
            if (fp == NULL)
                return NULL;

            if (ntohs(*fp) & ~0x7)
                break;

            hdrlen = 8;
        } else if (nexthdr == NEXTHDR_AUTH){
            hdrlen = (hp->hdrlen+2) << 2;
        } else {
            hdrlen = (hp->hdrlen+1) << 3;
        }

        nexthdr = hp->nexthdr;
        frag_off += hdrlen;
    }

    return (uint8_t *)ip + frag_off;
}

static inline uint32_t getTransportSizeIPv6(const IPv6Header *ip){
    return htons(ip->payload_len);
}

#endif
