#include <stdio.h>
#include "serial.h"
#include <unistd.h>
#include <sys/ioctl.h>

void stm_reboot_to_system_memory()
{
    serial_set_pin(TIOCM_RTS);
   // usleep(1000000);
    serial_set_pin(TIOCM_DTR);
    usleep(200000);
    serial_reset_pin(TIOCM_DTR);
    usleep(200000);
}
