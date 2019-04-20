#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdlib.h>
#include "serial.h"
#include <termios.h>
#include <sys/ioctl.h>

char* g_serial_name = NULL;
int g_serial_fd = -1;
int g_serial_open = 0;


static int serial_fd_get()
{
    return g_serial_fd;
}

static void serial_fd_set(const int fd)
{
    g_serial_fd = fd;
}

static char* serial_name_get()
{

    return g_serial_name;
}

static void serial_name_set(const char* const serial_name)
{
    if(NULL == serial_name)
    {
        return;
    }
    int str_len = 0;
    if(NULL != g_serial_name)
    {
        free(g_serial_name);
    }
    str_len = strlen(serial_name);
    if(g_serial_name = malloc(str_len + 1))
    {
    strncpy(g_serial_name, serial_name, str_len);
    }
}

static int serial_open(const char* const serial_name)
{
     int fd = 0;
     fd = open(serial_name, O_RDWR | O_NOCTTY | O_NDELAY);
     if(fd < 0)
     {
         perror("open");
         return -1;
     }

     if(fcntl(fd, F_SETFL, 0) <  0)
     {
         perror("fcntl F_SETFL 0");
         return -1;
     }

     if(isatty(fd) == 0)
     {
        perror("isatty");
        return -1;
     }

     serial_name_set(serial_name);
     serial_fd_set(fd);
     printf("serial %s open successful, fd=%d \n", serial_name, fd);
     g_serial_open = 1;
     return fd;
}

int serial_config(const SerialAtt* const serial_att, const int fd)
{
    struct termios new_tio = {0};
    struct termios old_tio = {0};

    if(tcgetattr(fd, &old_tio) != 0)
    {
        perror("tcgetattr");
        return -1;
    }

    new_tio.c_cflag |=  CLOCAL | CREAD;
    new_tio.c_cflag |=  ~CSIZE;

    switch(serial_att->bits)
    {
        case 8:
            new_tio.c_cflag |= CS8;
            break;
        default:
            new_tio.c_cflag |= CS7;
            break;
    }

    switch(serial_att->event)
    {
        case 'o':
        case 'O':
            new_tio.c_cflag |= PARENB;
            new_tio.c_cflag |= PARODD;
            new_tio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'e':
        case 'E':
            new_tio.c_cflag |= PARENB;
            new_tio.c_cflag &= ~PARODD;
            new_tio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'n':
        case 'N':
            new_tio.c_cflag &= ~PARENB;
            break;
        default:
            break;
    }

    switch(serial_att->speed)
    {
        case 2400:
            cfsetispeed(&new_tio, B2400);
            cfsetospeed(&new_tio, B2400);
            break;
        case 4800:
            cfsetispeed(&new_tio, B4800);
            cfsetospeed(&new_tio, B4800);
            break;
        case 115200:
            cfsetispeed(&new_tio, B115200);
            cfsetospeed(&new_tio, B115200);
            break;
        case 460800:
            cfsetispeed(&new_tio, B460800);
            cfsetospeed(&new_tio, B460800);
            break;
        default:
            cfsetispeed(&new_tio, B9600);
            cfsetospeed(&new_tio, B9600);
            break;
    }

    if(serial_att->stop == 1)
        new_tio.c_cflag &= ~CSTOPB;
    else
        new_tio.c_cflag |= CSTOPB;

    
    new_tio.c_cc[VTIME] = 0;
    new_tio.c_cc[VMIN] = 0;

    tcflush(fd,TCIFLUSH);

    if(tcsetattr(fd,TCSANOW,&new_tio) != 0);
    {
        perror("tcsetatr");
        return -1;
    }
    return 0;

}

int serial_allocate(const SerialAtt* const serial_att)
{
    int fd = 0;
    fd = serial_open(serial_att->serial_name);
    if(fd < 0)
        return -1;
    if(!serial_config(serial_att, fd))
    {
        return -1;
    }

    return 0;
}

int serial_release()
{
   if(g_serial_open)
   {
       if(-1 == close(g_serial_fd))
           perror("close");
   }
   g_serial_open = 0;
   printf("serial release ok!\n");
}

int serial_sent(const char* const data, const int length)
{
    char * buff = NULL;
    int len = 0;
    int ret = 0;

    if(NULL == data || length < 1)
    {
        printf("sent data error!\n");
        return -1;
    }
    if(!g_serial_open)
    {
        printf("the termi %s not open!\n", g_serial_name);
    }

    buff = (char*)data;
    len = length;

    while(len > 0)
    {
        ret = write(g_serial_fd, buff, len);
        if(ret < 1)
        {
            perror("write");
            return ret;
        }
        len -= ret;
        buff += ret;
    }

    return length - len;
}

int serial_receive( char* const data, const int length )
{
    fd_set fdr;
    struct timeval timeout = {3,0};
    int cnt = 0;
    char * buff;
    int ret = 0;
    
    if(NULL == data || length < 1)
    {
        printf("sent data error!\n");
        return -1;
    }
    if(!g_serial_open)
    {
        printf("the termi %s not open!\n", g_serial_name);
    }
    buff = data;
    while (cnt < length )
    { 
        FD_ZERO(&fdr);
        FD_SET(g_serial_fd, &fdr);
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        ret = select(g_serial_fd + 1, &fdr, NULL, NULL, &timeout);
        if ( ret == -1 ) 
        {   
            break;
        } 
        else if ( ret == 0 ) 
        { 
            break;
        } 
        else 
        { 
            if ( FD_ISSET(g_serial_fd, &fdr) )
            { 
                ret = read(g_serial_fd , buff+cnt , length-cnt);
                if ( ret >= 0 )
                { 
                    cnt += ret;
                }
            }
        }
    }   

    return 0;

}

 void serial_set_pin(int pin) {
     int set;
    if(!g_serial_open)
    {
        printf("the termi %s not open!\n", g_serial_name);
    }
     ioctl (g_serial_fd, TIOCMGET, &set);
     set |= pin;
     ioctl (g_serial_fd, TIOCMSET, &set);
 }

 void serial_reset_pin(int pin) {
     int set;
    if(!g_serial_open)
    {
        printf("the termi %s not open!\n", g_serial_name);
    }
     ioctl (g_serial_fd, TIOCMGET, &set);
     set &= ~pin;
     ioctl (g_serial_fd, TIOCMSET, &set);
 }



