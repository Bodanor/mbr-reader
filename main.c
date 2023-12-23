#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"

int main(int argc, char **argv)
{
    MBR *mbr = NULL;

    mbr = read_mbr("/dev/nvme0n1");
    print_whole_mbr(mbr);
}