#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printf("%s [port] [output (b)inary/ output (a)scii]\n", argv[0]);
        exit(0);
    }
    int termfd;
    WORD read_character;
    BYTE buffer = 'b';
    struct termios old_term_settings, new_term_settings;
    termfd = open(argv[1], O_RDWR);
    tcgetattr(termfd, &old_term_settings);
    memcpy(&new_term_settings, &old_term_settings, sizeof(struct termios));
    cfsetispeed(&new_term_settings, B9600);
    cfsetospeed(&new_term_settings, B9600);
    new_term_settings.c_cflag = 0;
    new_term_settings.c_iflag = 0;
    new_term_settings.c_lflag = 0;
    new_term_settings.c_oflag = 0;
    new_term_settings.c_cflag = CLOCAL | CREAD | CS8;
    new_term_settings.c_cc[VMIN] = 1;
    new_term_settings.c_cc[VTIME] = 0;
    tcsetattr(termfd, TCSAFLUSH, &new_term_settings);
    tcsetattr(termfd, TCSANOW, &new_term_settings);
    write(termfd, &buffer, 1);
    do
    {
        read(termfd, &read_character, sizeof(read_character));
        if(argv[2][0] == 'a')
            printf("%d\n",read_character);
        else if(argv[2][0] == 'b')
            printf("%c%c", read_character >> 8, read_character & 0xff);
    }while((char)read_character != 0);
    return 0;
}
