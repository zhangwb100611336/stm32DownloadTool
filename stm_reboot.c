#include <stdio.h>
#include "serial.h"
#include <unistd.h>
#include <sys/ioctl.h>

void stm_reboot_to_system_memory()
{
    serial_set_pin(TIOCM_RTS);
    serial_set_pin(TIOCM_DTR);
    sleep(1);
    serial_reset_pin(TIOCM_DTR);
}
