#ifndef PAIRING_H_INCLUDED
#define PAIRING_H_INCLUDED

#include "system_a4.h"

#define A4_PAIR_NONE    0x00FFFFFF
#define A4_PAIR_MOUSE   0x01
#define A4_PAIR_KBD     0x02

#define A4_PAIR_FIND_ON  1
#define A4_PAIR_FIND_OFF 0

#define A4_PAIR_MOD_OK        1
#define A4_PAIR_MOD_IO        0
#define A4_PAIR_MOD_ER_MAX   -1
#define A4_PAIR_MOD_EXIST    -2
#define A4_PAIR_MOD_INVALID  -3
#define A4_PAIR_MOD_WRITE    -4
#define A4_PAIR_MOD_NOTFND   -5

#define A4_PAIR_STATE_DISABLE       true
#define A4_PAIR_STATE_ENABLE        false


struct a4_pair_device
{
    int type;
    unsigned int ID;
};

struct a4_pair_devlist
{
    int type;
    unsigned int ID[5];
    bool disabled[5];
};

a4_pair_device a4_pair_get_new_device(a4_device *dev);
a4_pair_devlist a4_pair_get_list_mouse(a4_device *dev);
a4_pair_devlist a4_pair_get_list_keyboard(a4_device *dev);
int a4_pair_set_find_mode(a4_device *dev, unsigned short mod);

//int a4_pair_write_devices_ids(a4_device *dev, a4_pair_devlist mouse, a4_pair_devlist kbd);

int a4_pair_add_new_device(a4_device *dev, a4_pair_device new_device);
int a4_pair_add_new_mouse(a4_device *dev, unsigned int new_device_id);
int a4_pair_add_new_keybd(a4_device *dev, unsigned int new_device_id);

int a4_pair_del_device_by_id(a4_device *dev, unsigned int dev_id);
int a4_pair_del_mouse_by_index(a4_device *dev, int index);
int a4_pair_del_keybd_by_index(a4_device *dev, int index);

int a4_pair_switch_keybd_by_index(a4_device *dev, int index, bool state);
int a4_pair_switch_mouse_by_index(a4_device *dev, int index, bool state);
int a4_pair_switch_device_by_id(a4_device *dev, unsigned int dev_id, bool state);


const char *a4_pair_get_error_string(int error);

#endif // PAIRING_H_INCLUDED
