#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"

int main(int argc, char **argv)
{
    MBR *mbr = NULL;

    mbr = read_mbr("/home/liwinux/Downloads/debian-12.4.0-amd64-netinst.iso");
    print_whole_mbr(mbr);
}