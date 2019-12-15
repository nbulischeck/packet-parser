#include "main.h"
#include "listener.h"
#include "util.h"

int fanout_id;
int fanout_type;
int interface_num;

sig_atomic_t sigint = 0;

static void sighandler(int signal){
    switch(signal){
        case SIGINT:
        case SIGTERM:
        default:
            sigint = 1;
            break;
    }
}

static void parse_arguments(int argc, char **argv){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s INTERFACE {hash|lb|cpu}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fanout_id = getpid() & 0xffff;
    interface_num = get_if_index(argv[1]);

    if (!strcmp(argv[2], "hash")){
        fanout_type = PACKET_FANOUT_HASH;
    } else if (!strcmp(argv[2], "lb")){
        fanout_type = PACKET_FANOUT_LB;
    } else if (!strcmp(argv[2], "cpu")){
        fanout_type = PACKET_FANOUT_CPU;
    } else {
        fprintf(stderr, "Unknown fanout type [%s]\n", argv[2]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv){
    int i, num_cpus;
#ifdef DEBUG
    uint8_t *shmem;
    struct aggregate total = {0};
#endif
    struct tpacket_stats_v3 stats;

    parse_arguments(argc, argv);

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGHUP, sighandler);

    num_cpus = sysconf(_SC_NPROCESSORS_ONLN);

#ifdef DEBUG
    shmem = create_shared_memory(stats_size * num_cpus);
    if (shmem == MAP_FAILED){
        perror("mmap");
        exit(-1);
    }
#endif

    for (i = 0; i < num_cpus; i++){
        pid_t pid = fork();
        if (pid == 0){
            fanout_thread(&stats);
#ifdef DEBUG
            memcpy(shmem+(stats_size*i), &stats, stats_size);
#endif
            exit(0);
        } else if (pid == -1){
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < num_cpus; i++){
        int status;
        wait(&status);
    }

#ifdef DEBUG
    for (i = 0; i < num_cpus; i++){
        memcpy(&stats, shmem+(stats_size*i), stats_size);
        total.tp_packets += stats.tp_packets;
        total.tp_drops += stats.tp_drops;
        total.tp_freeze_q_cnt += stats.tp_freeze_q_cnt;
    }

    delete_shared_memory(shmem, stats_size * num_cpus);

    printf("\nReceived %llu packets, %llu dropped, freeze_q_cnt: %llu\n",
        total.tp_packets, total.tp_drops, total.tp_freeze_q_cnt);
#endif

    return 0;
}
