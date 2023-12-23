#include "terminal.h"

unsigned int get_term_x(void)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    return w.ws_col;

}
unsigned int get_term_y(void)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    return w.ws_row;

}

void print_ascii_header(const char *title)
{
    unsigned int columns;
    unsigned int rows;
    unsigned int title_size;
    unsigned int i;

    title_size = strlen(title);

    putchar('|');
    for (columns = 0; columns < get_term_x() - 2; columns++)
        putchar('-');
    putchar('|');
    printf("\n|");

    for (i = 0; i < (get_term_x() / 2) - (title_size/2); i++)
        putchar(' ');
    printf(title);
    for (; i < get_term_x() - 2 - (title_size); i++)
        putchar(' ');
    printf("|\n");
    putchar('|');

    for (columns = 0; columns < get_term_x() - 2; columns++)
        putchar('-');
    printf("|\n");


}