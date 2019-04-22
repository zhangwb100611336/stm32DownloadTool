#include "serial.h"
#include "stm_cmd.h"
#include "stm_reboot.h"
#include <unistd.h>
#include <stdio.h>

int main(int argn, char** argv)
{
    uint8_t rec_data[10] = {0};
    if(argn > 1)
    {
        printf("tty: %s\n", argv[1]);

        SerialAtt s;
        s.speed = 115200;
        s.serial_name = argv[1];
        s.bits = 8;
        s.stop = 1;
        s.event = 'E';

        int ret;
        printf("open serial....\n");
        ret = serial_allocate(&s);
        if(0 != ret)
        {
            printf(" allocate failed, ret=%d\n", ret);
            return 1;
        }
        
        printf(" reboot stm32...\n");
        stm_reboot_to_system_memory();

        printf("sync to stm32....\n");
        if(stm32_sync() == 0)
        {
            printf("sync successful!\n");
            printf("==================\n");
            stm32_cmd_get();
            printf("==================\n");
            stm32_cmd_get_gv();
            printf("==================\n");
            stm32_cmd_get_gid();
            printf("==================\n");
            stm32_cmd_rm(rec_data,0x8000000,8);
            printf("==================\n");
        }


        ret = serial_release() ;

        printf("release ret = %d\n", ret);
    }

    return 0;
}
