#include "pairing.h"


a4_pair_device a4_pair_get_new_device(a4_device *dev)
{
    a4_pair_device tmp;
    tmp.type = A4_PAIR_NONE;
    tmp.ID = 0xFFFFFFFF;

    if (dev)
    {
        unsigned char ret[8];
        int res = a4_dongle_read(dev, 0xB600 , 6, ret, 8);

        if (res != 8)
            return tmp;


        if ((ret[6] & 0x20) == 0x20) //new mouse
        {
            tmp.type = A4_PAIR_MOUSE;
            tmp.ID = ((ret[0] & 0x7) << 16 ) | (ret[1] << 8) | ret[2];
        }
        else if ((ret[6] & 0x40) == 0x40) //new keyboard
        {
            tmp.type = A4_PAIR_KBD;
            tmp.ID = ((ret[3] & 0x7) << 16 ) | (ret[4] << 8) | ret[5];
        }
    }

    return tmp;
}

int a4_pair_set_find_mode(a4_device *dev, unsigned short mod)
{
    if (mod == A4_PAIR_FIND_ON || mod == A4_PAIR_FIND_OFF)
        return a4_dongle_write(dev, 0xBF02, mod);
}

static unsigned short a4_pair_addr_mouse[5] = {0x2772, 0x2720, 0x2728, 0x2730, 0x2738};
static unsigned short a4_pair_addr_keybd[5] = {0x2774, 0x2740, 0x2748, 0x2750, 0x2758};
