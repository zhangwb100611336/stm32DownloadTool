#include "stm_cmd.h"
#include "serial.h"
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

#define ACK (uint8_t)(0x79)
#define NACK (uint8_t)(0x1f)

#define STM_SYNC_CMD (0x7F)
#define STM_SYNC_CMD_LENGTH (1)
#define STM_SYNC_TIMES (10)

#define STM_GET_CMD (uint8_t)(0x00)
#define STM_GET_GV (uint8_t)(0x01)
#define STM_GET_GID (uint8_t)(0x02)
#define STM_RM (uint8_t)(0x11)

static int stm32_cmd_sent(const uint8_t cmd)
{
    uint8_t cmd_send[2] = {0};
    cmd_send[0] = cmd;
    cmd_send[1] = cmd^(0xff);
    printf("cmd_send[0] = %0.2x\n", cmd_send[0]);
    printf("cmd_send[1] = %0.2x\n", cmd_send[1]);

    if(serial_sent(cmd_send, 2) == 2)
    {
            if(serial_receive(cmd_send,1) == 1)
            {
                if(cmd_send[0]== ACK)
                {
                    return 0;
                }
                else if(cmd_send[0] == NACK)
                {
                    printf("receive NACK!\n");
                    return -1;
                }
                else
                {
                    printf("unkonw ack :%0.2x\n",cmd_send[0]);
                    return -1;
                }
            }
    }
    printf("send failed!\n");
    return -1;
}

static int stm32_data_sent(uint8_t* data, uint8_t number)
{
    uint8_t rec_ack = 0;
    if(NULL == data)
        return -1;

    if(serial_sent(data, number) == number)
    {
            if(serial_receive(&rec_ack,1) == 1)
            {
                if(rec_ack== ACK)
                {
                    return number;
                }
                else if(rec_ack == NACK)
                {
                    printf("receive NACK!\n");
                    return -1;
                }
                else
                {
                    printf("unkonw ack :%0.2x\n",rec_ack);
                    return -1;
                }
            }
    }
    printf("send failed!\n");
    return -1;
}

int stm32_cmd_get()
{
    uint8_t rec_data[15] = {0};
    uint8_t index = 0;
    int i = 0;
    if(stm32_cmd_sent(STM_GET_CMD) != 0)
    {
        return -1;
    }
    
    while (index < 15)
    {
        
        if(serial_receive(rec_data + index, 1) == 1)
        {
            if(index == 0)
            {
                if(ACK == rec_data[0] || NACK == rec_data[0])
                {
                    return -1;
                }
            }
            else if(index < rec_data[0]+ 1)
            {
                if(ACK == rec_data[index] || NACK == rec_data[index])
                {
                    return -1;
                }
            }
            else
            {
                if(ACK == rec_data[index] || NACK == rec_data[index])
                {
                    break;
                }
            }
            index++;
        }
        printf("*");
        fflush(stdout);
    }
    
    for(i = 0; i < index ; i++)
    {
        printf("rec_data[%d] = %0.2x\n", i, rec_data[i]);
    }

    return  0;
}
int stm32_cmd_get_gv()
{
    uint8_t rec_data[4] = {0};
    uint8_t index = 0;
    int i = 0;
    if(stm32_cmd_sent(STM_GET_GV) != 0)
    {
        return -1;
    }

    while (index < 4)
    {
        if(serial_receive(rec_data + index, 1) == 1)
        {
            if(NACK == rec_data[index])
            {
                return -1;
            }
            if(ACK == rec_data[index])
            {
                break;
            }
            index++;
        }
    }
    for(i = 0; i < index ; i++)
    {
        printf("rec_data[%d] = %0.2x\n", i, rec_data[i]);
    }

    return  0;
}
int stm32_cmd_get_gid()
{
    uint8_t rec_data[4] = {0};
    uint8_t index = 0;
    int i = 0;
    if(stm32_cmd_sent(STM_GET_GID) != 0)
    {
        return -1;
    }

    while (index < 4)
    {
        if(serial_receive(rec_data + index, 1) == 1)
        {
            if(NACK == rec_data[index])
            {
                return -1;
            }
            if(ACK == rec_data[index])
            {
                break;
            }
            index++;
        }
    }
    for(i = 0; i < index ; i++)
    {
        printf("rec_data[%d] = %0.2x\n", i, rec_data[i]);
    }

    return  0;
}

int stm32_cmd_rm(uint8_t *data,uint32_t addr, uint8_t number)
{
    uint8_t index = 0;
    uint8_t send_addrss[5] = {0};
    uint8_t send_number[2] = {0};
    int i = 0;
    if(NULL == data || 0 == number)
    {
        printf("input para error\n");
        return -1;
    }
    if(stm32_cmd_sent(STM_RM) != 0)
    {
        return -1;
    }
    *((uint32_t*)send_addrss) =  htonl(addr);
    for(i = 0; i < 4 ; i++)
    {
        send_addrss[4] ^= send_addrss[i];
        printf("send_addrss[%d]= %0.2x\n",i, send_addrss[i]);
    }
    printf("send_addrss[%d]= %0.2x\n",i, send_addrss[i]);

    if(stm32_data_sent(send_addrss, 5) != 5)
    {
        printf("send address error!\n");
        return -1;
    }
    
    send_number[0] = number;
    send_number[1] = (~number);
    if(stm32_data_sent(send_number, 2) != 2)
    {
        printf("send number error!\n");
        return -1;
    }
    while (index < number+1)
    {
        if(serial_receive(data + index, 1) == 1)
        {
            if(NACK == data[index])
            {
                return -1;
            }
            if(ACK == data[index])
            {
                break;
            }
            index++;
        }
    }




       for(i = 0; i < index ; i++)
    {
        printf("data[%d] = %0.2x\n", i, data[i]);
    }

    return  0;
}

int stm32_sync()
{
     uint8_t stm_sys_cmd = STM_SYNC_CMD;
     int length = STM_SYNC_CMD_LENGTH;

    uint8_t rec_data = 0;
    int i = STM_SYNC_TIMES;
    int ret = 0;

    while(i > 0 )
    {
        ret = serial_sent(&stm_sys_cmd, length);
        if(ret > 0)
        {
            ret = serial_receive(&rec_data, length);
            //printf("ret = %0.2x\n", ret);
            if(length == ret)
            {
               // printf("rec_data = %0.2x\n", rec_data);
                if(rec_data == 0x79)
                    return 0;
            }
            
        }
        i--;
    }
    return -1;
}
