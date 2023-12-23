#include "mbr.h"

static short check_boot_signature(MBR *mbr);
static short is_valid_partition(partitionEntry partition);

static short check_boot_signature(MBR *mbr)
{
    if (mbr != NULL) {
        if (mbr->mbr_boot_signature == 0xAA55) /* This is the only way to tell if its a valid MBR */
            return 0;
        else
            fprintf(stderr, "The path provided does not point to a MBR structure (Boot signature missing) !");
    }
    return -1;
}

static short is_valid_partition(partitionEntry partition)
{
    if (partition.partition_number_of_sectors == 0 || partition.partition_lba_first_absolute_sector == 0)
        return 1;
    else
        return 0;
    
}

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
        fprintf(stderr, "Could not load the whole MBR structure. Is the path correctly pointing to an MBR disk ?\n");
        free(mbr);
        return NULL;
    }

    if (check_boot_signature(mbr) != 0) {
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
    unsigned long remaining_bytes = 0;

    /* NULL pointer is also verified in the bellow function */
    if (check_boot_signature(mbr) != 0)
        return -1;
    
    i = 0;
    while (i < sizeof(mbr->mbr_bootstrap_code)) {
        /* 446 is not a multiple of 16, so there will be some remaining bytes. Print just those */
        if (i + sizeof(current_bytes) > sizeof(mbr->mbr_bootstrap_code))
            remaining_bytes = sizeof(mbr->mbr_bootstrap_code) - i;

        /* Copy the current 16 bytes into the array */
        memcpy(current_bytes, mbr->mbr_bootstrap_code, sizeof(current_bytes) - remaining_bytes);
        if (show_addresses)
            printf("%08lx: ", i);

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

void print_chs_first_absolute_sector(partitionEntry partition)
{
    printf("\tCylinder %d: \n", partition.partition_chs_first_absolute_sector[2]);
    printf("\tHead : %d\n", partition.partition_chs_first_absolute_sector[0]);
    printf("\tSector : %d\n", partition.partition_chs_first_absolute_sector[1]);
    printf("\n");
}

void print_chs_last_absolute_sector(partitionEntry partition)
{
    printf("\tCylinder : %d\n", partition.partition_chs_last_absolute_sector[2]);
    printf("\tHead : %d\n", partition.partition_chs_last_absolute_sector[0]);
    printf("\tSector : %d\n", partition.partition_chs_last_absolute_sector[1]);
    printf("\n");

}
short get_number_of_partition_entries(MBR *mbr)
{
    unsigned long i;
    uint8_t valid_partition_count;


    if (check_boot_signature(mbr) != 0) {
        return -1;
    }
    
    for (i = 0; i < MAX_PARTITION_COUNT; i++) {
        if (is_valid_partition(*(&mbr->partition1 + (i*sizeof(partitionEntry)))) == 0)
            valid_partition_count++;
    }
    
    return valid_partition_count;
}

void print_partition_status(partitionEntry partition)
{
    printf("\tInactive(0x00) | Active(0x80) | Invalid (0x01-0x7F)\n");
    printf("\tGot status : 0x%02x --> ", partition.partition_status);

    if (partition.partition_status == 0)
        printf("Inactive");
    else if (partition.partition_status == 0x80)
        printf("Active");
    else
        printf("Invalid");

    printf("\n\n");
}
void print_lba_first_absolute_partition(partitionEntry partition)
{
    printf("\tLBA of the first absolute partition : %d\n", partition.partition_lba_first_absolute_sector);
    printf("\n");
}
void print_partition_sector_count(partitionEntry partition)
{
    printf("\tSectors : %d\n", partition.partition_number_of_sectors);
    printf("\n");
}

void print_partition_info(partitionEntry partition)
{
    printf("Partition status : \n\n");
    print_partition_status(partition);
    printf("CHS address of the first absolute sector in partition : \n\n");
    print_chs_first_absolute_sector(partition);
    printf("Partition Type : \n\n");
    printf("CHS address of the last absolute sector in partition : \n\n");
    print_chs_last_absolute_sector(partition);
    printf("LBA of the first absolute sector in the partition : \n\n");
    print_lba_first_absolute_partition(partition);
    printf("Numbers of sectors in the partition : \n\n");
    print_partition_sector_count(partition);
    printf("\n\n");
}

void print_all_partition_info(MBR *mbr)
{
    uint8_t partition_number;
    char title_buffer[50];

    for (partition_number = 0; partition_number < MAX_PARTITION_COUNT; partition_number++) {
        if (is_valid_partition(*(&mbr->partition1 + (partition_number*sizeof(partitionEntry)))) == 0) {
            sprintf(title_buffer, "PARTITION NUMBER : %d", partition_number + 1); /* We want to start from partition 1 */
            print_ascii_header(title_buffer);
            print_partition_info(*(&mbr->partition1 + (partition_number*sizeof(partitionEntry))));
        }
    }
}

short print_whole_mbr(MBR *mbr)
{
    /* NULL pointer is also verified in the bellow function */
    if (check_boot_signature(mbr) != 0)
        return -1;
    
    print_ascii_header("BOOTSTRAP CODE : ");
    print_bootstrap_code(mbr, 1);
    print_all_partition_info(mbr);
}