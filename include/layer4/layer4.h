#ifndef LAYER4_H
#define LAYER4_H

#include <arpa/inet.h>

#include "../util.h"

enum L4_Type {
    TYPE_TCP  = 1,
    TYPE_UDP  = 2,
    TYPE_ICMP = 3
};

typedef struct transport_interface {
    uint16_t (*_getSrcPort)(const void *instance);
    uint16_t (*_getDstPort)(const void *instance);
} TransportInterface;

typedef struct icmp_interface {
    uint8_t (*_getControlType)(const void *instance);
    uint8_t (*_getControlCode)(const void *instance);
} ICMPInterface;

typedef struct l4_interface {
    TransportInterface *tp;
    ICMPInterface *icmp;
    uint32_t (*_getDataLength)(const void *instance, uint32_t packet_size);
    uint8_t *(*_getDataPtr)(const void *instance);
} L4Interface;

typedef struct {
    void *instance;
    const L4Interface *interface;
    uint32_t packet_size;
    uint8_t layer4_type;
} L4Header;

L4Header L4(void *instance, L4Interface *interface, uint32_t packet_size, uint8_t layer4_type){
    return (L4Header){instance, interface, packet_size, layer4_type};
}

inline uint8_t getL4Type(L4Header *l4){
    return l4->layer4_type;
}

uint16_t getSrcPort(L4Header *l4){
    if (!l4->interface->tp)
        return -1;
    return (l4->interface->tp->_getSrcPort)(l4->instance);
}

uint16_t getDstPort(L4Header *l4){
    if (!l4->interface->tp)
        return -1;
    return (l4->interface->tp->_getDstPort)(l4->instance);
}

uint16_t getControlType(L4Header *l4){
    if (!l4->interface->icmp)
        return -1;
    return (l4->interface->icmp->_getControlType)(l4->instance);
}

uint16_t getControlCode(L4Header *l4){
    if (!l4->interface->icmp)
        return -1;
    return (l4->interface->icmp->_getControlCode)(l4->instance);
}

inline uint32_t getDataLength(L4Header *l4){
    return (l4->interface->_getDataLength)(l4->instance, l4->packet_size);
}

inline uint8_t *getDataPtr(L4Header *l4){
    return (l4->interface->_getDataPtr)(l4->instance);
}

#include "tcp.h"
#include "udp.h"
#include "icmp.h"

L4Interface *TCP_vtable = &(L4Interface) {
    .tp = &(TransportInterface) {
        ._getSrcPort = (uint16_t (*)(const void *)) getSrcPortTCP,
        ._getDstPort = (uint16_t (*)(const void *)) getDstPortTCP
    },
    .icmp = NULL,
    ._getDataLength = (uint32_t (*)(const void *, uint32_t)) getDataLengthTCP,
    ._getDataPtr = (uint8_t *(*)(const void *)) getDataPtrTCP
};

L4Interface *UDP_vtable = &(L4Interface) {
    .tp = &(TransportInterface) {
        ._getSrcPort = (uint16_t (*)(const void *)) getSrcPortUDP,
        ._getDstPort = (uint16_t (*)(const void *)) getDstPortUDP
    },
    .icmp = NULL,
    ._getDataLength = (uint32_t (*)(const void *, uint32_t)) getDataLengthUDP,
    ._getDataPtr = (uint8_t *(*)(const void *)) getDataPtrUDP
};

L4Interface *ICMP_vtable = &(L4Interface) {
    .tp = NULL,
    .icmp = &(ICMPInterface) {
        ._getControlType = (uint8_t (*)(const void *)) getControlTypeICMP,
        ._getControlCode = (uint8_t (*)(const void *)) getControlCodeICMP
    },
    ._getDataLength = (uint32_t (*)(const void *, uint32_t)) getDataLengthICMP,
    ._getDataPtr = (uint8_t *(*)(const void *)) getDataPtrICMP
};

#endif
