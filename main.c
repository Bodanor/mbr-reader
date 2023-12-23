#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"

int main(int argc, char **argv)
{
    MBR *mbr = NULL;

    mbr = read_mbr("disk.img");
    print_bootstrap_code(mbr, 1);

}