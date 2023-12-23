#include "mbr.h"

static short check_boot_signature(MBR *mbr);

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

MBR *read_mbr(const char *path)
{
    int mbr_fd;
    int bytes_read;
    MBR *mbr = NULL;

    mbr_fd = open(path, O_RDONLY);
    if (mbr_fd == -1){
        perror("Could not read MBR structure. Reason ");
        return NULL;
    }

    mbr = (MBR*)malloc(sizeof(MBR));
    if (mbr == NULL) {
        perror("Malloc error, abording now... ");
        return NULL;
    }

    bytes_read = read(mbr_fd, mbr, sizeof(MBR));
    if (bytes_read != sizeof(MBR)) {
        fprintf(stderr, "Could not load the whole MBR structure. Is the path correctly pointing to a disk ?");
        free(mbr);
        return NULL;
    }

    if (check_boot_signature(mbr) != 0) {
        fprintf(stderr, "The path provided does not point to a MBR structure (Boot signature missing) !");
        free(mbr);
        return NULL;
    }

    close(mbr_fd);

    return mbr;
}

short print_bootstrap_code(MBR *mbr, uint8_t show_addresses)
{
    uint8_t current_bytes[16];
    unsigned long i;
    unsigned long j;
    int remaining_bytes = 0;

    if (mbr == NULL || check_boot_signature(mbr) != 0)
        return -1;
    
    i = 0;
    while (i < sizeof(mbr->mbr_bootstrap_code)) {
        /* 446 is not a multiple of 16, so there will be some remaining bytes. Print just those */
        if (i + sizeof(current_bytes) > sizeof(mbr->mbr_bootstrap_code))
            remaining_bytes = sizeof(mbr->mbr_bootstrap_code) - i;

        /* Copy the current 16 bytes into the array */
        memcpy(current_bytes, mbr->mbr_bootstrap_code, sizeof(current_bytes) - remaining_bytes);
        if (show_addresses)
            printf("%08x: ", i);

        /* Print the hexadecimal values of the 16 bytes array */
        for (j = 0; j < sizeof(current_bytes) - remaining_bytes; j++)
            printf("%02hhX ", mbr->mbr_bootstrap_code[i + j]);

        /* For alignement purposes, if we are at the end of the bootstrap code */
        if (remaining_bytes)
            for (j = 0; j < remaining_bytes; j++)
                printf("   ");

        /* Print the ascii values of the current 16 bytes array */
        for (j = 0; j < sizeof(current_bytes) - remaining_bytes; j++){
            if (isprint(mbr->mbr_bootstrap_code[i + j]))
                printf("%c", mbr->mbr_bootstrap_code[i + j]);
            else
                printf(".");
        }

        /* Newline when finished and increment i by 16 bytes */
        putchar('\n');
        i+=sizeof(current_bytes);
    }

    return 0;

}
static short check_boot_signature(MBR *mbr)
{
    if (mbr != NULL) {
        if (mbr->mbr_boot_signature == 0xAA55) /* This is the only way to tell if its a valid MBR */
            return 0;
    }
    return -1;
}