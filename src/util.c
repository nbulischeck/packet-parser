#include "main.h"
#include "util.h"

int get_if_index(char *device_name){
    struct ifreq ifr;
    size_t if_name_len;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0){
        perror("socket");
        exit(1);
    }

    if_name_len = strlen(device_name)+1;
    if (if_name_len > sizeof(ifr.ifr_name)){
        printf("Interface name too long.");
        exit(-1);
    }

    memcpy(ifr.ifr_name, device_name, if_name_len);

    if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1){
        perror("ioctl");
        exit(-1);
    }

    return ifr.ifr_ifindex;
}

void *create_shared_memory(size_t size){
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_SHARED;
    return mmap(NULL, size, prot, flags, 0, 0);
}

inline void delete_shared_memory(void *map, size_t size){
    munmap(map, size);
}

char *bytes_to_macstr(const uint8_t *bytes, size_t size){
    char *macstr = malloc(size*3), *ptr = &macstr[0];

    for (size_t i = 0; i < size-1; i++)
        ptr += sprintf(ptr, "%02X:", bytes[i]);

    ptr += sprintf(ptr, "%02X", bytes[size-1]);
    *ptr = '\0';

    return macstr;
}

char *bytes_to_hexstr(const uint8_t *bytes, size_t size){
    char *hexstr = malloc(size*2 + 1), *ptr = &hexstr[0];

    for (size_t i = 0; i < size; i++)
        ptr += sprintf(ptr, "%02X", bytes[i]);
    *ptr = '\0';

    return hexstr;
}
