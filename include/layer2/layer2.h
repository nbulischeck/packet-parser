#ifndef LAYER2_H
#define LAYER2_H

#include <arpa/inet.h>

#include "../util.h"

typedef struct l2_interface {
    char *(*_getSrcMAC)(const void *instance);
    char *(*_getDstMAC)(const void *instance);
    uint16_t (*_getL3Proto)(const void *instance);
    uint8_t *(*_getL3Header)(const void *instance);
} L2Interface;

typedef struct {
    const void *instance;
    const L2Interface *interface;
} L2Header;

inline L2Header L2(void *instance, L2Interface *interface){
    return (L2Header){instance, interface};
}

inline char *getL2SrcMAC(L2Header *l2){
    return (l2->interface->_getSrcMAC)(l2->instance);
}

inline char *getL2DstMAC(L2Header *l2){
    return (l2->interface->_getDstMAC)(l2->instance);
}

inline uint16_t getL3Proto(L2Header *l2){
    return (l2->interface->_getL3Proto)(l2->instance);
}

inline uint8_t *getL3Header(L2Header *l2){
    return (l2->interface->_getL3Header)(l2->instance);
}

#include "eth.h"

L2Interface *ETH_vtable = &(L2Interface) {
    ._getSrcMAC = (char *(*)(const void *)) getSrcETHMAC,
    ._getDstMAC = (char *(*)(const void *)) getDstETHMAC,
    ._getL3Proto = (uint16_t (*)(const void *)) getL3ProtoETH,
    ._getL3Header = (uint8_t *(*)(const void *)) getL3HeaderETH
};

#endif
