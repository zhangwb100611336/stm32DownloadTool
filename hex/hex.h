#ifndef __HEX_H__
#define __HEX_H__
#include <unistd.h>
#include <stdint.h>

typedef struct hexLine_S
{
    struct hexLine_S * next;
    uint8_t len;
    uint16_t addr;
    uint32_t real_addr;
    uint8_t rec;
    uint8_t checksum;
    uint8_t* data;
}hexLine;

typedef struct hexFile_S
{
    struct hexFile_S* next;
    uint8_t instance_id;
    char* file_name;
    hexLine* lines_header;
}hexFile;

void hex_file_print(uint8_t instance_id);
uint8_t hex_file_create(char * file_name);
hexFile* hex_file_get_id(uint8_t instance_id);
//read file
//read data

#endif
