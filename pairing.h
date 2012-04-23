#ifndef PAIRING_H_INCLUDED
#define PAIRING_H_INCLUDED

#include "system_a4.h"

#define A4_PAIR_NONE    0xFFFFFFFF
#define A4_PAIR_MOUSE   0x01
#define A4_PAIR_KBD     0x02

#define A4_PAIR_FIND_ON  1
#define A4_PAIR_FIND_OFF 0

struct a4_pair_device
{
    int type;
    int ID;
};


a4_pair_device a4_pair_get_new_device(a4_device *dev);
int a4_pair_set_find_mode(a4_device *dev, unsigned short mod);

#endif // PAIRING_H_INCLUDED
