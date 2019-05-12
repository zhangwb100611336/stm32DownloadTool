#include "hex.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

hexFile* files_header = NULL;
uint8_t instance_id_seed = 0;
static uint8_t line_data_check(hexLine* line)
{
    if(line == NULL)
        return -1;
    uint8_t checksum = 0;
    uint8_t index = 0;
    checksum += line->len;
    checksum += ((uint8_t*)(&line->addr))[0];
    checksum += ((uint8_t*)(&line->addr))[1];
    checksum += line->rec;
    checksum += line->checksum;
    while(index<line->len)
    {
        checksum += line->data[index];
        index++;
    }

    return checksum;
}
static hexLine* hex_file_read_line(FILE* fd)
{
    uint8_t ch = 0;
    hexLine * line = NULL;
    uint8_t index = 0;
    if(NULL == fd)
    {
        return NULL;
    }
    while(1)
    {
        ch = fgetc(fd);
        if(ch == ':')
        {
           break;
        }
    }
    if(NULL == (line = (hexLine*)malloc(sizeof(hexLine))))
    {
        perror("malloc hexline");
        goto ERRORRETURN;
    }
    
    fscanf(fd,"%02x",&line->len);
    fscanf(fd,"%04x",&line->addr);
    fscanf(fd,"%02x",&line->rec);
    if(line->len != 0)
    {
        if(NULL == (line->data = (uint8_t*)malloc(line->len)))
        {
            perror("malloc data");
            goto ERRORRETURN;
        }
        while(index < line->len)
        {
            fscanf(fd,"%02x",&(line->data[index]));
            index++ ;
        }
    }
    fscanf(fd,"%02x",&(line->checksum)); 
    
    if(line_data_check(line))
    {
        printf("%s:%d chech sum failed!\n",__FILE__,__LINE__);
        goto ERRORRETURN;
    }

    return line;

ERRORRETURN:
    if(NULL != line)
    {
        if(line->data)
            free(line->data);
        free(line);
    }
    return NULL;
}

static int line_add_list(hexLine** header, hexLine* new_node)
{
    if(NULL == new_node || NULL == header)
    {
        printf("%s:%d input para error!\n",__FILE__,__LINE__);
        return -1;
    }

    if(NULL == (*header))
    {
        *header = new_node;
        return 0;
    }

    hexLine * node = NULL;
    node = *header;
    while(node->next)
    {
        node = node->next;
    }
    node->next = new_node;
    return 0;
}

static int line_destory_list(hexLine* header)
{
    if(NULL == header)
        return 0;
    hexLine * node = NULL;
    node = header;
    while(node)
    {
        node = header->next;
        if(header->data)
            free(header->data);
        free(header);
        header = node;
    }

    return 0;
}

static hexLine* hex_file_read(FILE *fd)
{
    hexLine* line_list_header = NULL;
    hexLine* line = NULL;
    uint32_t base_addr = 0;
    uint8_t finish_flag = 1;
    if(NULL == fd)
        return NULL;

    while(finish_flag)
    {
        if(line=hex_file_read_line(fd))
        {
            switch(line->rec)
            {
                case 0x00:
                    line->real_addr = line->addr + base_addr;
                    line_add_list(&line_list_header,line);
                    break;
                case 0x01:
                    finish_flag = 0;
                    break;
                case 0x02:
                    base_addr = 0;
                    base_addr = (uint32_t)ntohs(*((uint16_t*)line->data));
                    base_addr <<= 8;
                    break;
                case 0x03:
                    base_addr = 0;
                    base_addr = (uint32_t)ntohs(*((uint16_t*)line->data));
                    base_addr <<= 4;
                    base_addr += (uint32_t)ntohs(*((uint16_t*)&line->data[2]));
                    break;
                case 0x04:
                    base_addr = 0;
                    base_addr = (uint32_t)ntohs(*((uint16_t*)line->data));
                    base_addr <<= 16;
                    break;
                case 0x05:
                    base_addr = 0;
                    base_addr = (uint32_t)ntohl(*((uint32_t*)line->data));
                    break;
                default:
                    printf("%s:%d case default!\n",__FILE__,__LINE__);
                    goto ERRORRETURN;
                    break;
            }
        }
        else 
        {
            printf("%s:%d read line failed!\n",__FILE__,__LINE__);
            goto ERRORRETURN;
        }
    }
    return line_list_header;
ERRORRETURN:
    line_destory_list(line_list_header);
    return NULL;
    
}

static inline uint8_t instance_id_generator()
{
   return instance_id_seed++;
}

static uint8_t add_hex_file(hexLine* line,char* file_name)
{
    if(NULL == line || NULL == file_name)
        return -1;
    
    hexFile* file = NULL;
    hexFile* hex_file = malloc(sizeof(hexFile));
    if(NULL == hex_file)
    {
        printf("%s:%d input para error!\n",__FILE__,__LINE__);
        return -1;
    }
    hex_file->lines_header = line ;
    hex_file->instance_id = instance_id_generator();
    hex_file->file_name = malloc(strlen(file_name) + 1);
    if(hex_file->file_name)
    {
        strcpy(hex_file->file_name,file_name);
    }
    else
    {
        printf("%s:%d malloc hex file error!\n",__FILE__,__LINE__);
        return -1;
    }

    if(NULL==files_header)
    {
        files_header = hex_file;
    }
    else
    {
        file = files_header;
        while(file->next)
        {
            file = file->next;
        }
        file->next = hex_file;
    }

    return hex_file->instance_id;

}
uint8_t hex_file_create(char * file_name)
{
    hexLine * line =NULL;
    uint8_t instance_id = -1;
    if(NULL == file_name)
    {
        printf("%s:%d file name is null\n",__FILE__,__LINE__);
        return -1;
    }
    printf("file_name :%s\n",file_name);
    FILE* fp = fopen(file_name,"r");
    if(NULL == fp)
    {
        perror("fopen");
        return -1;
    }
 

    if(line = hex_file_read(fp))
    {
       if(-1 == (instance_id = add_hex_file(line, file_name)))
       {
            printf("%s:%d add hex file error\n",__FILE__,__LINE__);
           line_destory_list(line);
           return -1;
       }
    }
    else
    {
        printf("%s:%d hex file read error!\n",__FILE__,__LINE__);
    }

    return instance_id; 
}

hexFile* hex_file_get_id(uint8_t instance_id)
{
    hexFile* file = NULL;
    file = files_header;
    while(file)
    {
        if(instance_id == file->instance_id)
            return file;
        file = file->next;
    }
    return NULL;
}

void hex_line_print(hexLine * line)
{
        
    if(NULL == line)
        return ;
    printf("0x%02x 0x%04x 0x%08x 0x%02x 0x%02x ",
            line->len, line->addr, line->real_addr, line->rec, line->checksum);
    uint8_t i = 0;
    while(i < line->len)
    {
        printf("0x%02x ",line->data[i]);
        i++;
    }
    printf("\n");
    return;
}

void hex_file_print(uint8_t instance_id)
{
    hexFile* file = NULL;
    uint8_t max = 0;
    char ch = 0;
    if(NULL ==(file = hex_file_get_id(instance_id)))
    {
        return ;
    }
    hexLine* line = file->lines_header;
    printf("len   addr  rel_addr   rec    checksum    data\n");
    while(line)
    {
        hex_line_print(line);
        line = line->next;
        max ++ ;
        if(max > 10)
        {
            ch = getchar();
            if(ch == 'q')
                break;
        }
    }
    printf("\n");
    return ;
}


