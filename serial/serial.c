#include<fcntl.h>
#include<stdint.h>
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
     printf("fd = %d\n",fd);
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
	struct termios optnew;
	speed_t speed = B19200;


		/* Get current option */
		tcgetattr(fd, &optnew);

		/* initialize new option to raw input/output */
//		memset(&optnew, 0, sizeof(optnew));
		cfmakeraw(&optnew);
		optnew.c_cc[VMIN ] = 0;
		optnew.c_cc[VTIME] = 2*10;

		/* set baudrate */
		switch (serial_att->speed) {
			case   1200: speed =   B1200;  break;
			case   1800: speed =   B1800;  break;
			case   4800: speed =   B4800;  break;
			case   9600: speed =   B9600;  break;
			case  19200: speed =  B19200;  break;
			case  38400: speed =  B38400;  break;
			case  57600: speed =  B57600;  break;
			case 115200: speed = B115200;  break;
			default:    speed = B19200; break;
		}
		cfsetispeed(&optnew, speed);
		cfsetospeed(&optnew, speed);

		/* Set data bits */
		optnew.c_cflag &= ~CSIZE;
		optnew.c_cflag &= ~CRTSCTS;
		optnew.c_iflag &= ~(ICRNL|IXON);
		optnew.c_cflag |= CLOCAL | CREAD;
		optnew.c_oflag &= ~OPOST;

		switch (serial_att->bits) {
			case 5: optnew.c_cflag |= CS5; break;
			case 6: optnew.c_cflag |= CS6; break;
			case 7: optnew.c_cflag |= CS7; break;
			default :
				optnew.c_cflag |= CS8; break;
		}

		/* Set parity checking */
		optnew.c_cflag |= PARENB;
		switch (serial_att->event) {
			case 'e':
			case 'E': optnew.c_cflag &= ~PARODD; break;
			case 'o':
			case 'O': optnew.c_cflag &=  PARODD; break;
			default :
				optnew.c_cflag &= ~PARENB; break;
		}

		/* Set stop bit(s) */
		if (serial_att->stop == 2)
			optnew.c_cflag &=  CSTOPB;
		else
			optnew.c_cflag &= ~CSTOPB;
	
    	optnew.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG );

		/* Apply new option */
		tcsetattr(fd, TCSANOW, &optnew);
	return 0;
}

int serial_allocate(const SerialAtt* const serial_att)
{
    int fd = 0;
    fd = serial_open(serial_att->serial_name);
    if(fd < 0)
        return -1;
    if(serial_config(serial_att, fd) != 0)
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

int serial_sent( uint8_t*  data, int length)
{
    uint8_t * buff = NULL;
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

    buff = (uint8_t*)data;
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

int serial_receive( uint8_t* data,  int length )
{
    fd_set fdr;
    struct timeval timeout = {3,0};
    int cnt = 0;
    uint8_t * buff;
    int ret = 0;
    
    if(NULL == data || length < 1)
    {
        printf("sent data error!\n");
        return -1;
    }
    if(!g_serial_open)
    {
        printf("the termi %s not open!\n", g_serial_name);
        return -1;
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

    return ret;

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

 void serial_flush()
 {
     tcflush(g_serial_fd, TCIOFLUSH);
 }


