#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
int main()
{
    int fd = fileno(stdin);
    char c;
 
    /* gotoxy(current_x + 20, current_y - 3) */
    printf("%c[3A%c[20C", 27, 27);
 
    /* query cursor position */
    printf("%c[6n", 27);
    fflush(stdout);
 
    /* trick, drain first char (0x27) of terminal input */
    read(fd, &c, 1);
	
	printf("\n\n\n\nc= '%s'", c);
    /* you can see "[x;yR" */
 
    return 0;
}