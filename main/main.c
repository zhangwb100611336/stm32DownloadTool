#include "test.h"
#include "serial.h"
#include "stm_cmd.h"
#include "stm_reboot.h"
#include <unistd.h>
#include <stdio.h>
#include <hex.h>



int burn(int argn, char** argv)
{

    int ret;
    uint8_t instance_id = -1;
    uint32_t begin_addr = 0;
    hexFile* file = NULL;
    hexLine* line = NULL;
    if(argn < 1)
        return 0;

    SerialAtt s;
    s.speed = 115200;
    s.serial_name = argv[1];
    s.bits = 8;
    s.stop = 1;
    s.event = 'E';
    
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
    if(stm32_sync() != 0)
    {
        printf("sync failed!\n");
        return 0;
    }
    
   if(-1 == (instance_id = hex_file_create(argv[2]) ))
   {
       printf("creat hex file failed!\n");
       return 0;
   }

   if(-1 == stm32_cmd_er(NULL,0))
   {
       printf("earse momery failed!\n");
        return 0;   
   }

   file = hex_file_get_id(instance_id);
   if(NULL == file)
   {
        printf("get hex file failed!\n");
        return 0;
   }

   line = file->lines_header;
    if(NULL == line)
    {
        printf("line is NULL!\n");
        return 0;
    }

    printf("begin write to memory\n");
    begin_addr = line->real_addr;
    while(line)
    {
       if(-1 == stm32_cmd_wm(line->data,line->real_addr,line->len))
       {
           printf("write memory error!\n");
           return 0;
       }
       line = line->next;
       printf(".");
    }

    printf("\nwrite complete!\n");

    printf("run program\n");
    sleep(1);
    if(-1 == stm32_cmd_go(begin_addr))
    {
        printf("go command error!\n");
    }
    else
    {
        printf("successful go command!\n");
    }
    

}


int main(int argn, char ** argv)
{
//  return  hex_test(argn,argv);
//   return stm_test(argn,argv);
    burn(argn,argv);
}
