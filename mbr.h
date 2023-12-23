#ifndef __MBR_H__
#define __MBR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

typedef struct mbr_t MBR;
typedef struct mbr_partition_entry_t partitionEntry;

MBR *read_mbr(const char *path);
short print_bootstrap_code(MBR *mbr, uint8_t show_addresses);
#endif