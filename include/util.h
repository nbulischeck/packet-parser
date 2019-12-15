#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <net/if.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define NO_PROTO 255

#define SUPPRESS_UNUSED(x) (void)(x)

int get_if_index(char *device_name);
void *create_shared_memory(size_t size);
void delete_shared_memory(void *map, size_t size);
char *bytes_to_hexstr(const uint8_t *bytes, size_t size);
char *bytes_to_macstr(const uint8_t *bytes, size_t size);

#endif
