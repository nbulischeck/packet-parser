#ifndef ARP_H
#define ARP_H

#include <stdint.h>
#include <linux/if_arp.h>

#define ARP_LEN sizeof(struct arphdr)

typedef struct arphdr ARPHeader;

inline ARPHeader *ARP(const uint8_t *packet){
    return (ARPHeader *)packet;
}

static char *getSenderARPMAC(const ARPHeader *arp){
    return bytes_to_macstr((uint8_t *)arp + ARP_LEN, arp->ar_hln);
}

static char *getTargetARPMAC(const ARPHeader *arp){
    return bytes_to_macstr(
        (uint8_t *)arp \
        + ARP_LEN \
        + arp->ar_hln \
        + arp->ar_pln, arp->ar_hln);
}

static inline uint16_t getL4ProtoARP(__attribute__ ((unused)) ARPHeader *arp){
    return NO_PROTO;
}

static inline uint8_t *getL4HeaderARP(__attribute__ ((unused)) ARPHeader *arp){
    return NULL;
}

static inline uint32_t getTransportSizeARP(__attribute__ ((unused)) ARPHeader *arp){
    return 0;
}

#endif
