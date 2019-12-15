#include "main.h"
#include "listener.h"
#include "parser.h"
#include "db.h"

extern int fanout_id;
extern int fanout_type;
extern int interface_num;
extern sig_atomic_t sigint;

static void teardown_socket(struct ring *ring, int fd){
    munmap(ring->map, ring->req.tp_block_size * ring->req.tp_block_nr);
    free(ring->rd);
    close(fd);
}

static int setup_socket(struct ring *ring){
    int fd, err, fanout_arg;
    const unsigned int framesiz = 1 << 11;
    const unsigned int blocksiz = 1 << 22;
    const unsigned int blocknum = 64;
    const struct sockaddr_ll ll = {
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = interface_num,
        .sll_hatype = 0,
        .sll_pkttype = 0,
        .sll_halen = 0,
        .sll_addr = {0}
    };

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd < 0){
        perror("socket");
        exit(1);
    }

    err = setsockopt(fd, SOL_PACKET, PACKET_VERSION,
                    &(int){TPACKET_V3}, sizeof(int));
    if (err < 0){
        perror("setsockopt");
        exit(1);
    }

    ring->req.tp_block_size = blocksiz;
    ring->req.tp_frame_size = framesiz;
    ring->req.tp_block_nr = blocknum;
    ring->req.tp_frame_nr = (blocksiz * blocknum) / framesiz;
    ring->req.tp_retire_blk_tov = 60;
    ring->req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;

    err = setsockopt(fd, SOL_PACKET, PACKET_RX_RING,
                    &ring->req, sizeof(ring->req));
    if (err < 0){
        perror("setsockopt");
        exit(1);
    }

    ring->map = mmap(NULL, ring->req.tp_block_size * ring->req.tp_block_nr,
                    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);
    if (ring->map == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    ring->rd = malloc(ring->req.tp_block_nr * sizeof(*ring->rd));
    assert(ring->rd);

    for (unsigned int i = 0; i < ring->req.tp_block_nr; ++i){
        ring->rd[i].iov_base = ring->map + (i * ring->req.tp_block_size);
        ring->rd[i].iov_len = ring->req.tp_block_size;
    }

    err = bind(fd, (struct sockaddr *) &ll, sizeof(ll));
    if (err < 0){
        perror("bind");
        exit(1);
    }

    fanout_arg = (fanout_id | (fanout_type << 16));
    err = setsockopt(fd, SOL_PACKET, PACKET_FANOUT,
             &fanout_arg, sizeof(fanout_arg));
    if (err){
        perror("setsockopt");
        return EXIT_FAILURE;
    }

    return fd;
}

static inline void flush_block(struct block_desc *pbd){
    pbd->h1.block_status = TP_STATUS_KERNEL;
}

static void walk_block(struct block_desc *pbd){
    int i, num_pkts = pbd->h1.num_pkts;
    struct tpacket3_hdr *ppd;

    ppd = (struct tpacket3_hdr *)((uint8_t *)pbd+pbd->h1.offset_to_first_pkt);
    for (i = 0; i < num_pkts; ++i){
        parse_packet(ppd);
        ppd = (struct tpacket3_hdr *)((uint8_t *)ppd+ppd->tp_next_offset);
    }
}

void fanout_thread(struct tpacket_stats_v3 *stats){
    int fd, err;
    socklen_t len;
    struct ring ring;
    struct pollfd pfd;
    unsigned int block_num = 0, blocks = 64;
    struct block_desc *pbd;

    memset(&ring, 0, sizeof(ring));
    fd = setup_socket(&ring);
    assert(fd > 0);

    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fd;
    pfd.events = POLLIN | POLLERR;
    pfd.revents = 0;

    while (likely(!sigint)){
        pbd = (struct block_desc *) ring.rd[block_num].iov_base;

        if ((pbd->h1.block_status & TP_STATUS_USER) == 0) {
            poll(&pfd, 1, -1);
            continue;
        }

        walk_block(pbd);
        flush_block(pbd);
        block_num = (block_num + 1) % blocks;
    }

    len = sizeof(*stats);
    err = getsockopt(fd, SOL_PACKET, PACKET_STATISTICS, stats, &len);
    if (err < 0) {
        perror("getsockopt");
        exit(1);
    }

    teardown_socket(&ring, fd);
}
