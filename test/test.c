#include "serial.h"
#include "stm_cmd.h"
#include "stm_reboot.h"
#include <unistd.h>
#include <stdio.h>
#include <hex.h>

void print_data(uint8_t* data, uint8_t length )
{
    if(NULL == data)
        return ;
    printf("length: %d\n",length);
    uint8_t index = 0;
    for(index = 0; index < length; index++)
    {
        if(!index%16)
        {
            if(0 != index)
                printf("\n");
            printf("%0.2x ： ",index);
        }

        printf("0x%0.2x ",data[index]);
    }

    printf("\n");
    return;
}


int stm_test(int argn, char** argv)
{
    uint8_t test_data[100] = {0};
    uint8_t length = 0;
    if(argn > 1)
    {

        SerialAtt s;
        s.speed = 115200;
        s.serial_name = argv[1];
        s.bits = 8;
        s.stop = 1;
        s.event = 'E';

        int ret;
        printf("begin test program.\n");
        printf("begin test serial:\n");
        printf("trying to allocate serial %s\n",argv[1]);
        ret = serial_allocate(&s);
        sleep(1);
        if(0 != ret)
        {
            printf(" allocate failed, ret=%d\n", ret);
            return 1;
        }
        printf("allocate successful, test serial ok!\n");
        
        printf(" reboot mcu to system memory:\n");
        stm_reboot_to_system_memory();

        printf("sync with mcu\n");
        if(stm32_sync() == 0)
        {
            printf("sync successful!\n");
            printf("==================\n");
            printf("get the cmd what mcu support:\n");
            if(-1 == stm32_cmd_get(test_data,&length))
            {
                printf("get cmd failed! please check this ....\n");
            }
            else
            {
                printf("successful get support cmd:\n");
                print_data(test_data,length);
            }
            printf("==================\n");
            if(-1 == stm32_cmd_get_gv(test_data,&length))
            {
                printf("get gv failed! please check this ....\n");
            }
            else
            {
                printf("successful get gv:\n");
                print_data(test_data,length);
            }
            printf("==================\n");
           if(-1 == stm32_cmd_get_gid(test_data,&length))
            {
                printf("get gv failed! please check this ....\n");
            }
            else
            {
                printf("successful get gv:\n");
                print_data(test_data,length);
            }
            printf("==================\n");
            if(-1 == stm32_cmd_rm(test_data,0x8000000,8))
            {
                printf("read momery error!\n");
            }
            else
            {
                printf("successful read memory:\n");
                print_data(test_data,8);
            }
            printf("==================\n");
         if(-1 == stm32_cmd_er(NULL,0))
            {
                printf("earse momery error!\n");
            }
            else
            {
                printf("successful earse memory:\n");
            }
            printf("==================\n");
           if(-1 == stm32_cmd_rm(test_data,0x8000000,9))
            {
                printf("read momery error!\n");
            }
            else
            {
                printf("successful read memory:\n");
                print_data(test_data,9);
            }
            printf("==================\n");
            uint8_t wm_data[8] = {1,2,3,4,5,6,7,8};
         if(-1 == stm32_cmd_wm(wm_data,0x8000000,8))
            {
                printf("write momery error!\n");
            }
            else
            {
                printf("successful write memory:\n");
                print_data(wm_data,8);
            }
            printf("==================\n");
         if(-1 == stm32_cmd_rm(test_data,0x8000000,9))
            {
                printf("read momery error!\n");
            }
            else
            {
                printf("successful read memory:\n");
                print_data(test_data,9);
            }
            printf("==================\n");


            /*
            sleep(10);
             if(-1 == stm32_cmd_go(0x8000000))
            {
                printf("go command error!\n");
            }
            else
            {
                printf("successful go command!\n");
            }
            */    
            printf("==================\n");
        }
        else
        {
            printf("sync failed:\n 1.please check the mcu whether reboot to system momery.\n  2.check whether send right message.\n ");
        }

        printf("begin release serial!\n");
        ret = serial_release() ;
        if(-1== ret)
            printf("release serial failed!\n");
        printf("release successful!\n");
    }

    return 0;
}

int hex_test(int argn, char** argv)
{
   if(argn < 1)
       return 0;

   printf("hex file name:%s\n",argv[1]);
   uint8_t instance_id = -1;
   if(-1 == (instance_id = hex_file_create(argv[1]) ))
   {
       return 0;
   }
   hex_file_print(instance_id);
   return 0;
}

   
