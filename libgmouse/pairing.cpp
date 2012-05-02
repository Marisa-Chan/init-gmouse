#include "pairing.h"
#include "control_a4.h"
#include "mouse_memory.h"
#include <stdio.h>


a4_pair_device a4_pair_get_new_device(a4_device *dev)
{
    a4_pair_device tmp;
    tmp.type = A4_PAIR_NONE;
    tmp.ID = A4_PAIR_NONE;

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
    if (!dev)
        return A4_ERROR;

    if (mod == A4_PAIR_FIND_ON || mod == A4_PAIR_FIND_OFF)
        return a4_dongle_write(dev, 0xBF02, mod);

    return A4_ERROR;
}

static unsigned short a4_pair_addr_mouse[5] = {0x2772, 0x2720, 0x2728, 0x2730, 0x2738};
static unsigned short a4_pair_addr_keybd[5] = {0x2774, 0x2740, 0x2748, 0x2750, 0x2758};

a4_pair_devlist a4_pair_get_list_mouse(a4_device *dev)
{
    a4_pair_devlist tmp;
    tmp.type = A4_PAIR_MOUSE;

    if (!dev)
    {
        tmp.type = A4_PAIR_NONE;
        for (int i=0; i<5 ; i++)
        {
            tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
            tmp.ID[i] = A4_PAIR_NONE;
        }
        return tmp;
    }


    for (int i=0; i<5 ; i++)
    {
        unsigned char buf[4];
        tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
        if (a4_mem_read_block(dev,a4_pair_addr_mouse[i],2,buf,NULL) == A4_SUCCESS)
        {
            tmp.disabled[i] = A4_PAIR_STATE_ENABLE;
            tmp.ID[i] = (buf[1] << 24) | (buf[0] << 16) | (buf[3] << 8) | buf[2];

            if ((0x80 & buf[0]) == 0x80)
                tmp.disabled[i] = A4_PAIR_STATE_DISABLE;

            if (a4_id_check(tmp.ID[i]) == A4_ERROR)
            {
                tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
                tmp.ID[i] = A4_PAIR_NONE;
            }
            else
                tmp.ID[i] &= 0x7FFFF;

        }
        else
        {
            tmp.ID[i] = A4_PAIR_NONE;
            tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
        }
    }

    return tmp;
}

a4_pair_devlist a4_pair_get_list_keyboard(a4_device *dev)
{
    a4_pair_devlist tmp;
    tmp.type = A4_PAIR_KBD;

    if (!dev)
    {
        tmp.type = A4_PAIR_NONE;
        for (int i=0; i<5 ; i++)
        {
            tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
            tmp.ID[i] = A4_PAIR_NONE;
        }
        return tmp;
    }

    for (int i=0; i<5 ; i++)
    {
        unsigned char buf[4];
        tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
        if (a4_mem_read_block(dev,a4_pair_addr_keybd[i],2,buf,NULL) == A4_SUCCESS)
        {
            tmp.disabled[i] = A4_PAIR_STATE_ENABLE;
            tmp.ID[i] = (buf[1] << 24) | (buf[0] << 16) | (buf[3] << 8) | buf[2];

            if ((0x80 & buf[0]) == 0x80)
                tmp.disabled[i] = A4_PAIR_STATE_DISABLE;

            if (((0xFF7FFFFF & tmp.ID[i]) != (tmp.ID[i] & 0x7FFFF)) || tmp.ID[i] == A4_PAIR_NONE || tmp.ID[i] == 0)
            {
                tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
                tmp.ID[i] = A4_PAIR_NONE;
            }
            else
                tmp.ID[i] &= 0x7FFFF;

        }
        else
        {
            tmp.ID[i] = A4_PAIR_NONE;
            tmp.disabled[i] = A4_PAIR_STATE_DISABLE;
        }
    }

    return tmp;
}

int a4_pair_write_devices_ids(a4_device *dev, a4_pair_devlist mouse, a4_pair_devlist kbd)
{
    unsigned short ms_addr[5] = {0x72, 0x20, 0x28, 0x30, 0x38};
    unsigned short kb_addr[5] = {0x74, 0x40, 0x48, 0x50, 0x58};

    if (!dev || mouse.type != A4_PAIR_MOUSE || kbd.type != A4_PAIR_KBD)
        return A4_ERROR;

    unsigned short mem[0x80];
    a4_mem_read_block(dev,0x2700,0x80,mem,NULL);

    for (int i=0; i<5; i++)
    {
        if (a4_id_check(kbd.ID[i]) == A4_ERROR ) //ignore incorrect devices
        {
            kbd.ID[i] = A4_PAIR_NONE;
            kbd.disabled[i] = A4_PAIR_STATE_ENABLE;
        }

        mem[kb_addr[i]]   = (kbd.ID[i] & 0xFFFF0000) >> 16;
        mem[kb_addr[i]+1] = (kbd.ID[i] & 0xFFFF);
        if (kbd.disabled[i])
            mem[kb_addr[i]] |= 0x80;

        if (a4_id_check(mouse.ID[i]) == A4_ERROR ) //ignore incorrect devices
        {
            mouse.ID[i] = A4_PAIR_NONE;
            mouse.disabled[i] = A4_PAIR_STATE_ENABLE;
        }

        mem[ms_addr[i]]   = (mouse.ID[i] & 0xFFFF0000) >> 16;
        mem[ms_addr[i]+1] = (mouse.ID[i] & 0xFFFF);
        if (mouse.disabled[i])
            mem[ms_addr[i]] |= 0x80;
    }

    //write new config to dongle

    if (a4_lock_mouse(dev,A4_LOCK_ON) == A4_ERROR)
        return A4_ERROR;

    if (a4_mem_write_block(dev,0x2700,0x80,mem, NULL) == A4_ERROR)
        return A4_ERROR;

    if (a4_lock_mouse(dev,A4_LOCK_OFF) == A4_ERROR)
        return A4_ERROR;

    return A4_SUCCESS;
}

int a4_pair_add_new_device(a4_device *dev, a4_pair_device new_device)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (new_device.type != A4_PAIR_KBD && new_device.type != A4_PAIR_MOUSE)
        return A4_PAIR_MOD_INVALID;

    if (a4_id_check(new_device.ID) == A4_ERROR)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    for (int i=0; i<5; i++)
        if (ms.ID[i] == new_device.ID || kb.ID[i] == new_device.ID)
            return A4_PAIR_MOD_EXIST;

    if (new_device.type == A4_PAIR_MOUSE)
    {
        for (int i=0; i<5; i++)
            if (ms.ID[i] == A4_PAIR_NONE || ms.ID[i] == 0)
            {
                ms.disabled[i] = A4_PAIR_STATE_ENABLE;
                ms.ID[i]       = new_device.ID;

                if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                    return A4_PAIR_MOD_OK;
                else
                    return A4_PAIR_MOD_WRITE;
            }
    }
    else if (new_device.type == A4_PAIR_KBD)
    {
        for (int i=0; i<5; i++)
            if (kb.ID[i] == A4_PAIR_NONE || kb.ID[i] == 0)
            {
                kb.disabled[i] = A4_PAIR_STATE_ENABLE;
                kb.ID[i]       = new_device.ID;

                if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                    return A4_PAIR_MOD_OK;
                else
                    return A4_PAIR_MOD_WRITE;
            }
    }

    return A4_PAIR_MOD_ER_MAX;
}

int a4_pair_add_new_mouse(a4_device *dev, unsigned int new_device_id)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(new_device_id) == A4_ERROR)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    for (int i=0; i<5; i++)
        if (ms.ID[i] == new_device_id || kb.ID[i] == new_device_id)
            return A4_PAIR_MOD_EXIST;

    for (int i=0; i<5; i++)
        if (ms.ID[i] == A4_PAIR_NONE || ms.ID[i] == 0)
        {
            ms.disabled[i] = A4_PAIR_STATE_ENABLE;
            ms.ID[i]       = new_device_id;

            if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                return A4_PAIR_MOD_OK;
            else
                return A4_PAIR_MOD_WRITE;
        }

    return A4_PAIR_MOD_ER_MAX;
}

int a4_pair_add_new_keybd(a4_device *dev, unsigned int new_device_id)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(new_device_id) == A4_ERROR)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    for (int i=0; i<5; i++)
        if (ms.ID[i] == new_device_id || kb.ID[i] == new_device_id)
            return A4_PAIR_MOD_EXIST;

    for (int i=0; i<5; i++)
        if (kb.ID[i] == A4_PAIR_NONE || kb.ID[i] == 0)
        {
            kb.disabled[i] = A4_PAIR_STATE_ENABLE;
            kb.ID[i]       = new_device_id;

            if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                return A4_PAIR_MOD_OK;
            else
                return A4_PAIR_MOD_WRITE;
        }

    return A4_PAIR_MOD_ER_MAX;
}

int a4_pair_del_device_by_id(a4_device *dev, unsigned int dev_id)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(dev_id) == A4_ERROR)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    for (int i=0; i<5; i++)
    {
        if (ms.ID[i] == dev_id)
        {
            ms.disabled[i] = A4_PAIR_STATE_ENABLE;
            ms.ID[i]       = A4_PAIR_NONE;

            if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                return A4_PAIR_MOD_OK;
            else
                return A4_PAIR_MOD_WRITE;
        }
        else if (kb.ID[i] == dev_id)
        {
            kb.disabled[i] = A4_PAIR_STATE_ENABLE;
            kb.ID[i]       = A4_PAIR_NONE;

            if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                return A4_PAIR_MOD_OK;
            else
                return A4_PAIR_MOD_WRITE;
        }
    }

    return A4_PAIR_MOD_NOTFND;
}

int a4_pair_del_mouse_by_index(a4_device *dev, int index)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (index < 0 || index > 4)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(ms.ID[index]) == A4_ERROR)
        return A4_PAIR_MOD_NOTFND;

    ms.disabled[index] = A4_PAIR_STATE_ENABLE;
    ms.ID[index]       = A4_PAIR_NONE;

    if (a4_pair_write_devices_ids(dev,ms,kb) != A4_SUCCESS)
        return A4_PAIR_MOD_WRITE;

    return A4_PAIR_MOD_OK;
}

int a4_pair_del_keybd_by_index(a4_device *dev, int index)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (index < 0 || index > 4)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(kb.ID[index]) == A4_ERROR)
        return A4_PAIR_MOD_NOTFND;

    kb.disabled[index] = A4_PAIR_STATE_ENABLE;
    kb.ID[index]       = A4_PAIR_NONE;

    if (a4_pair_write_devices_ids(dev,ms,kb) != A4_SUCCESS)
        return A4_PAIR_MOD_WRITE;

    return A4_PAIR_MOD_OK;
}

int a4_pair_switch_device_by_id(a4_device *dev, unsigned int dev_id, bool state)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(dev_id) == A4_ERROR)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    for (int i=0; i<5; i++)
    {
        if (ms.ID[i] == dev_id)
        {
            if (ms.disabled[i] == state)
                return A4_PAIR_MOD_OK;

            ms.disabled[i] = state;

            if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                return A4_PAIR_MOD_OK;
            else
                return A4_PAIR_MOD_WRITE;
        }
        else if (kb.ID[i] == dev_id)
        {
            if (kb.disabled[i] == state)
                return A4_PAIR_MOD_OK;

            kb.disabled[i] = state;

            if (a4_pair_write_devices_ids(dev,ms,kb) == A4_SUCCESS)
                return A4_PAIR_MOD_OK;
            else
                return A4_PAIR_MOD_WRITE;
        }
    }

    return A4_PAIR_MOD_NOTFND;
}

int a4_pair_switch_mouse_by_index(a4_device *dev, int index, bool state)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (index < 0 || index > 4)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(ms.ID[index]) == A4_ERROR )
        return A4_PAIR_MOD_NOTFND;

    if (ms.disabled[index] == state)
        return A4_PAIR_MOD_OK;

    ms.disabled[index] = state;

    if (a4_pair_write_devices_ids(dev,ms,kb) != A4_SUCCESS)
        return A4_PAIR_MOD_WRITE;

    return A4_PAIR_MOD_OK;
}

int a4_pair_switch_keybd_by_index(a4_device *dev, int index, bool state)
{
    if (!dev)
        return A4_PAIR_MOD_IO;

    if (index < 0 || index > 4)
        return A4_PAIR_MOD_INVALID;

    a4_pair_devlist ms = a4_pair_get_list_mouse(dev);
    a4_pair_devlist kb = a4_pair_get_list_keyboard(dev);

    if (ms.type != A4_PAIR_MOUSE || kb.type != A4_PAIR_KBD)
        return A4_PAIR_MOD_IO;

    if (a4_id_check(kb.ID[index]) == A4_ERROR)
        return A4_PAIR_MOD_NOTFND;

    if (kb.disabled[index] == state)
        return A4_PAIR_MOD_OK;

    kb.disabled[index] = state;

    if (a4_pair_write_devices_ids(dev,ms,kb) != A4_SUCCESS)
        return A4_PAIR_MOD_WRITE;

    return A4_PAIR_MOD_OK;
}

static const char *a4_pair_errstr[] =
{
    "Success",
    "Write error",
    "This device already added",
    "Fail, maximum 5 mouses and 5 keyboards",
    "IO errors",
    "Invalid device ID",
    "Not found",
    "Unknown error!",
    ""
};

const char *a4_pair_get_error_string(int error)
{
    switch (error)
    {
    case A4_PAIR_MOD_OK:
        return a4_pair_errstr[0];
    case A4_PAIR_MOD_WRITE:
        return a4_pair_errstr[1];
    case A4_PAIR_MOD_EXIST:
        return a4_pair_errstr[2];
    case A4_PAIR_MOD_ER_MAX:
        return a4_pair_errstr[3];
    case A4_PAIR_MOD_IO:
        return a4_pair_errstr[4];
    case A4_PAIR_MOD_INVALID:
        return a4_pair_errstr[5];
    case A4_PAIR_MOD_NOTFND:
        return a4_pair_errstr[6];
    default:
        return a4_pair_errstr[7];
    }

    return a4_pair_errstr[8];
}

