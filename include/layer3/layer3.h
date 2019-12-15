#ifndef LAYER3_H
#define LAYER3_H

#include <arpa/inet.h>

#include "../util.h"

enum L3_Type {
    TYPE_IPv4 = 1,
    TYPE_IPv6 = 2,
    TYPE_ARP  = 3
};

typedef struct ip_interface {
    char *(*_getSrcIP)(const void *instance);
    char *(*_getDstIP)(const void *instance);
} IPInterface;

typedef struct arp_interface {
    char *(*_getSenderMAC)(const void *instance);
    char *(*_getTargetMAC)(const void *instance);
} ARPInterface;

typedef struct l3_interface {
    IPInterface *ip;
    ARPInterface *arp;
    uint16_t (*_getL4Proto)(const void *instance);
    uint8_t *(*_getL4Header)(const void *instance);
    uint32_t (*_getTransportSize)(const void *instance);
} L3Interface;

typedef struct {
    void *instance;
    const L3Interface *interface;
    uint8_t layer3_type;
} L3Header;

L3Header L3(void *instance, L3Interface *interface, uint8_t layer3_type){
    return (L3Header){instance, interface, layer3_type};
}

inline uint8_t getL3Type(L3Header *l3){
    return l3->layer3_type;
}

char *getSrcIP(L3Header *l3){
    if (!l3->interface->ip)
        return NULL;
    return (l3->interface->ip->_getSrcIP)(l3->instance);
}

char *getDstIP(L3Header *l3){
    if (!l3->interface->ip)
        return NULL;
    return (l3->interface->ip->_getDstIP)(l3->instance);
}

char *getL3SrcMAC(L3Header *l3){
    if (!l3->interface->arp)
        return NULL;
    return (l3->interface->arp->_getSenderMAC)(l3->instance);
}

char *getL3DstMAC(L3Header *l3){
    if (!l3->interface->arp)
        return NULL;
    return (l3->interface->arp->_getTargetMAC)(l3->instance);
}

inline uint16_t getL4Proto(L3Header *l3){
    return (l3->interface->_getL4Proto)(l3->instance);
}

inline uint8_t *getL4Header(L3Header *l3){
    return (l3->interface->_getL4Header)(l3->instance);
}

inline uint32_t getTransportSize(L3Header *l3){
    return (l3->interface->_getTransportSize)(l3->instance);
}

#include "arp.h"
#include "ipv4.h"
#include "ipv6.h"

L3Interface *IPV4_vtable = &(L3Interface) {
    .ip = &(IPInterface) {
        ._getSrcIP = (char *(*)(const void *)) getSrcIPv4,
        ._getDstIP = (char *(*)(const void *)) getDstIPv4
    },
    .arp = NULL,
    ._getL4Proto = (uint16_t (*)(const void *)) getL4ProtoIPv4,
    ._getL4Header = (uint8_t *(*)(const void *)) getL4HeaderIPv4,
    ._getTransportSize = (uint32_t (*)(const void *)) getTransportSizeIPv4
};

L3Interface *IPV6_vtable = &(L3Interface) {
    .ip = &(IPInterface) {
        ._getSrcIP = (char *(*)(const void *)) getSrcIPv6,
        ._getDstIP = (char *(*)(const void *)) getDstIPv6
    },
    .arp = NULL,
    ._getL4Proto = (uint16_t (*)(const void *)) getL4ProtoIPv6,
    ._getL4Header = (uint8_t *(*)(const void *)) getL4HeaderIPv6,
    ._getTransportSize = (uint32_t (*)(const void *)) getTransportSizeIPv6
};

L3Interface *ARP_vtable = &(L3Interface) {
    .ip = NULL,
    .arp = &(ARPInterface) {
        ._getSenderMAC = (char *(*)(const void *)) getSenderARPMAC,
        ._getTargetMAC = (char *(*)(const void *)) getTargetARPMAC
    },
    ._getL4Proto = (uint16_t (*)(const void *)) getL4ProtoARP,
    ._getL4Header = (uint8_t *(*)(const void *)) getL4HeaderARP,
    ._getTransportSize = (uint32_t (*)(const void *)) getTransportSizeARP
};

#endif
