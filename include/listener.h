#ifndef IDS_H
#define IDS_H

#include <poll.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

#ifndef likely
    #define likely(x)   __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
    #define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#ifndef PACKET_FANOUT
    #define PACKET_FANOUT          18
    #define PACKET_FANOUT_HASH      0
    #define PACKET_FANOUT_LB        1
    #define PACKET_FANOUT_CPU       2
#endif

#define stats_size sizeof(struct tpacket_stats_v3)

struct block_desc {
    uint32_t version;
    uint32_t offset_to_priv;
    struct tpacket_hdr_v1 h1;
};

struct ring {
    struct iovec *rd;
    uint8_t *map;
    struct tpacket_req3 req;
};

struct aggregate {
    unsigned long long tp_packets;
    unsigned long long tp_drops;
    unsigned long long tp_freeze_q_cnt;
};

void fanout_thread(struct tpacket_stats_v3 *stats);

#endif
