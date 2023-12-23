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

#include "terminal.h"

#define MAX_PARTITION_COUNT 4

typedef struct mbr_t MBR;
typedef struct mbr_partition_entry_t partitionEntry;

struct mbr_partition_entry_t {
    uint8_t partition_status;
    uint8_t partition_chs_first_absolute_sector[3];
    uint8_t partition_type;
    uint8_t partition_chs_last_absolute_sector[3];
    uint32_t partition_lba_first_absolute_sector;
    uint32_t partition_number_of_sectors;
    
} __attribute__((packed));

struct mbr_t {
    uint8_t mbr_bootstrap_code[446];
    partitionEntry partition1;
    partitionEntry partition2;
    partitionEntry partition3;
    partitionEntry partition4;
    uint16_t mbr_boot_signature;
}__attribute__((packed));


MBR *read_mbr(const char *path);

/* Main MBR related*/
short print_bootstrap_code(MBR *mbr, uint8_t show_addresses);
short get_number_of_partition_entries(MBR *mbr);

/* Partition Entries related */
/* Specific properties */
void print_chs_first_absolute_sector(partitionEntry partition);
void print_chs_last_absolute_sector(partitionEntry partition);
void print_partition_status(partitionEntry partition);
void print_lba_first_absolute_partition(partitionEntry partition);
void print_partition_sector_count(partitionEntry partition);

/*All properties */
void print_partition_info(partitionEntry partition);
void print_all_partition_info(MBR *mbr);
short print_whole_mbr(MBR *mbr);

short print_summary(MBR *mbr);
#endif