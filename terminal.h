#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>

unsigned int get_term_x(void);
unsigned int get_term_y(void);
void print_ascii_header(const char *title);

#endif