#ifndef __STM_CMD_H__
#define __STM_CMD_H__
#include <stdint.h>
int stm32_sync();
int stm32_cmd_get(uint8_t* rec_data, uint8_t* data_length);
int stm32_cmd_get_gv();
int stm32_cmd_get_gid();
int stm32_cmd_rm(uint8_t *data,uint32_t addr, uint8_t number);
#endif
