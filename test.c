#include "serial.h"
#include <unistd.h>
#include <stdio.h>

int main(int argn, char** argv)
{
    if(argn > 1)
    {
        printf("tty: %s\n", argv[1]);

        SerialAtt s;
        s.speed = 9600;
        s.serial_name = argv[1];
        s.bits = 7;
        s.stop = 1;
        s.event = 'E';

        int ret;
        ret = serial_allocate(&s);
        printf(" allocate ret = %d\n", ret);
        sleep(60);
        ret = serial_release() ;

        printf("release ret = %d\n", ret);
    }

    return 0;
}
