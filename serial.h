#ifndef __SERIAL_H__
#define __SERIAL_H__

typedef struct SerialAtt_S
{
    char* serial_name;
    int speed;
    int bits;
    int stop;
    char event;
}SerialAtt;

int serial_allocate(const SerialAtt* const serial_att);
int serial_release();
void serial_set_pin(int pin);
void serial_reset_pin(int pin);
#endif
