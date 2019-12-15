#include "main.h"
#include "listener.h"
#include "db.h"

#include "layer2/layer2.h"
#include "layer3/layer3.h"
#include "layer4/layer4.h"

extern uint32_t num_rules;
extern struct rules *rules;

void print_packet(L2Header *l2, L3Header *l3, L4Header *l4){
    uint8_t proto_type;
    char *srcIP = NULL;
    char *dstIP = NULL;
    char *srcMAC = NULL;
    char *dstMAC = NULL;

    if (l4->instance) {
        proto_type = getL4Type(l4);
        switch (proto_type) {
        case TYPE_TCP:
        case TYPE_UDP:
            srcIP = getSrcIP(l3);
            dstIP = getDstIP(l3);

            printf("%s: ", proto_type == TYPE_TCP ? "TCP" : "UDP");
            printf("%s:%d -> %s:%d\n", srcIP, getSrcPort(l4), dstIP, getDstPort(l4));

            goto exit;
        case TYPE_ICMP:
            srcIP = getSrcIP(l3);
            dstIP = getDstIP(l3);

            printf("ICMP: %s -> %s | %d : %d\n", srcIP, dstIP, getControlType(l4), getControlCode(l4));

            goto exit;
        default:
            return;
        }
    } else if (l3->instance) {
        proto_type = getL3Type(l3);
        srcIP = getSrcIP(l3);
        dstIP = getDstIP(l3);

        printf("%d: %s -> %s\n", proto_type, srcIP, dstIP);
    } else if (l2->instance) {
        proto_type = getL3Type(l3);
        srcMAC = getL2SrcMAC(l2);
        dstMAC = getL2DstMAC(l2);

        printf("%d: %s -> %s\n", proto_type, srcMAC, dstMAC);
    }

exit:
    if (srcMAC) {
        free(srcMAC);
        srcMAC = NULL;
    }

    if (dstMAC) {
        free(dstMAC);
        dstMAC = NULL;
    }

    if (srcIP) {
        free(srcIP);
        srcIP = NULL;
    }

    if (dstIP) {
        free(dstIP);
        dstIP = NULL;
    }
}

void parse_packet(struct tpacket3_hdr *ppd){
    const void *packet;
    L2Header l2 = {0};
    L3Header l3 = {0};
    L4Header l4 = {0};

    packet = (uint8_t *)ppd + ppd->tp_mac;

    l2 = L2(ETH(packet), ETH_vtable);

    switch (getL3Proto(&l2)){
        case ETH_P_IP:
            l3 = L3(IPV4(getL3Header(&l2)), IPV4_vtable, TYPE_IPv4);
            break;
        case ETH_P_IPV6:
            l3 = L3(IPV6(getL3Header(&l2)), IPV6_vtable, TYPE_IPv6);
            break;
        case ETH_P_ARP:
            l3 = L3(ARP(getL3Header(&l2)), ARP_vtable, TYPE_ARP);
            break;
        case ETH_P_ECTP:
            /* Kamene test default */
        case NO_PROTO:
            goto check_rules;
        default:
            printf("l3proto err\n");
            return;
    };

    switch (getL4Proto(&l3)){
        case IPPROTO_ICMP:
            l4 = L4(ICMP(getL4Header(&l3)), ICMP_vtable, getTransportSize(&l3), TYPE_ICMP);
        case IPPROTO_ICMPV6:
            break; /* Not Implemented */
        case IPPROTO_TCP:
            l4 = L4(TCP(getL4Header(&l3)), TCP_vtable, getTransportSize(&l3), TYPE_TCP);
            break;
        case IPPROTO_UDP:
            l4 = L4(UDP(getL4Header(&l3)), UDP_vtable, getTransportSize(&l3), TYPE_UDP);
            break;
        case IPPROTO_NONE:
            /* No IPv6 NextHdr */
        case IPPROTO_HOPOPTS:
            /* Kamene test default */
        case NO_PROTO:
            goto check_rules;
        default:
            printf("l4proto err: %d | %d\n", getL3Type(&l3), getL4Proto(&l3));
            return;
    };

check_rules:
#ifdef DEBUG
    print_packet(&l2, &l3, &l4);
#else
    SUPPRESS_UNUSED(l4);
#endif
}
