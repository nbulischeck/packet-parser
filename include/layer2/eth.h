#ifndef ETH_H
#define ETH_H

#include <stdint.h>
#include <linux/if_ether.h>

#define VLAN_LEN sizeof(struct vlanhdr)
#define ETH_P_ECTP 0x9000 // Ethernet Configuration Test Protocol

struct vlanhdr {
    uint16_t vid:12;
    uint16_t dei:1;
    uint16_t pcp:3;
    uint16_t type;
} __attribute__ ((packed));

typedef struct {
    struct ethhdr eth;
    struct vlanhdr vlan;
} ETHHeader;

inline ETHHeader *ETH(const uint8_t *packet){
    return (ETHHeader *)packet;
}

static char *getSrcETHMAC(const ETHHeader *eth){
    return bytes_to_macstr(eth->eth.h_dest, ETH_ALEN);
}

static char *getDstETHMAC(const ETHHeader *eth){
    return bytes_to_macstr(eth->eth.h_source, ETH_ALEN);
}

static uint16_t getL3ProtoETH(const ETHHeader *eth){
    if (htons(eth->eth.h_proto) == ETH_P_8021Q)
        return htons(eth->vlan.type);
    return htons(eth->eth.h_proto);
}

static uint8_t *getL3HeaderETH(const ETHHeader *eth){
    if (htons(eth->eth.h_proto) == ETH_P_8021Q)
        return ((uint8_t *)eth + ETH_HLEN + VLAN_LEN);
    return ((uint8_t *)eth + ETH_HLEN);
}

#endif
