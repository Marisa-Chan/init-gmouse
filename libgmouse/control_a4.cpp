#include "control_a4.h"

#define OSCAR_RF_CHANNEL_1      0x0088
#define OSCAR_RF_CHANNEL_2      0x0094
#define OSCAR_RF_CHANNEL_3      0x0086
#define OSCAR_RF_CHANNEL_4      0x0081
#define OSCAR_RF_CHANNEL_5      0x0080
#define OSCAR_RF_CHANNEL_6      0x0089
#define OSCAR_RF_CHANNEL_7      0x008b
#define OSCAR_RF_CHANNEL_8      0x0090
#define OSCAR_RF_CHANNEL_9      0x0082
#define OSCAR_RF_CHANNEL_10     0x008e
#define OSCAR_RF_CHANNEL_11     0x009a
#define OSCAR_RF_CHANNEL_12     0x0084
#define OSCAR_RF_CHANNEL_13     0x0087
#define OSCAR_RF_CHANNEL_14     0x0095
#define OSCAR_RF_CHANNEL_AUTO   0x0015

static const uint8_t oscar_channels[15] = {OSCAR_RF_CHANNEL_AUTO, OSCAR_RF_CHANNEL_1,  OSCAR_RF_CHANNEL_2,  OSCAR_RF_CHANNEL_3,  OSCAR_RF_CHANNEL_4,
        OSCAR_RF_CHANNEL_5,    OSCAR_RF_CHANNEL_6,  OSCAR_RF_CHANNEL_7,  OSCAR_RF_CHANNEL_8,  OSCAR_RF_CHANNEL_9,
        OSCAR_RF_CHANNEL_10,   OSCAR_RF_CHANNEL_11, OSCAR_RF_CHANNEL_12, OSCAR_RF_CHANNEL_13, OSCAR_RF_CHANNEL_14
                                          };

static const uint8_t oscar_map_channel[32] = {5, 4, 9, 4, 12, 5, 3, 13, 1, 6, 6, 7, 1, 11, 10, 12, 8, 1, 7, 13, 2, 14, 12, 5, 9, 14, 11, 14, 3, 8, 10, 2};


int a4_rf_set_channel(a4_device *dev, int channel)
{
    if (!dev)
        return A4_ERROR;

    if (channel >= 0 && channel < 15)
        return a4_dongle_write(dev, 0xBE01, oscar_channels[channel]);

    return A4_ERROR;
}

a4_rf_channel a4_rf_get_channel(a4_device *dev)
{
    a4_rf_channel tmp;
    tmp.channel = A4_ERROR;
    tmp.type = A4_ERROR;

    if (!dev)
        return tmp;


    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0004, ret, 8);

    if (res != 8)
        return tmp;

    int raw_chan = ret[2] & 0x1F;
    tmp.channel = oscar_map_channel[raw_chan];
    if ((0x80 & ret[2]) == 0x80)
        tmp.type = A4_CHAN_MANUAL;
    else
        tmp.type = A4_CHAN_AUTO;

    return tmp;
}

int a4_rf_get_signal_level(a4_device *dev)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0004, ret, 8);

    if (res != 8)
        return A4_ERROR;

    return ret[1];
}


#define OSCAR_RF_DISTANCE_15    0x0016
#define OSCAR_RF_DISTANCE_20    0x009F

int a4_rf_set_distance(a4_device *dev, int distance)
{
    if (!dev)
        return A4_ERROR;

    if (distance == OSCAR_RF_DISTANCE_20 || distance == OSCAR_RF_DISTANCE_15 || distance == 15 || distance == 20)
    {
        if (distance == 15)
            distance = OSCAR_RF_DISTANCE_15;
        else if (distance == 20)
            distance = OSCAR_RF_DISTANCE_20;

        return a4_dongle_write(dev, 0xB61D, distance);
    }

    return A4_ERROR;
}


int a4_rf_get_distance(a4_device *dev)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600 , 0x0007, ret, 8);

    if (res != 8)
        return A4_ERROR;

    int tmp = (ret[6] == OSCAR_RF_DISTANCE_20 ? 20 : 15);

    return tmp;
}


int a4_wake_set_mode(a4_device *dev, a4_wake_mode mode)
{
    if (!dev)
        return A4_ERROR;

    if (mode.type == A4_WAKE_BY_CLICK
            ||
            mode.type == A4_WAKE_BY_MOVE
#ifdef A4_WAKE_BY_UNK
            ||
            mode.type == A4_WAKE_BY_UNK
#endif
       )
    {
        //same with oscar editor
        if (mode.time > 0x0a)
            mode.time = 0x0a;
        else if (mode.time < 0x03)
            mode.time = 0x03;

        uint16_t setting = (mode.type & 0xC0) | (mode.time & 0x3F);

        return a4_dongle_write(dev, 0xB61A , setting);
    }
    return A4_ERROR;
}


a4_wake_mode a4_wake_get_mode(a4_device *dev)
{
    a4_wake_mode tmp;
    tmp.time = A4_ERROR;
    tmp.type = A4_ERROR;

    if (!dev)
        return tmp;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0007, ret, 8);

    if (res != 8)
        return tmp;

    tmp.time = ret[7] & 0x3F;
    tmp.type = ret[7] & 0xC0;

    if ((tmp.type & 0x80) == 0x80)
    {
        tmp.time = 10;
        tmp.type = A4_WAKE_BY_CLICK;
    }

    return tmp;
}

int a4_lock_mouse(a4_device *dev, unsigned short lock)
{
    if (!dev)
        return A4_ERROR;

    if (lock == A4_LOCK_ON || lock == A4_LOCK_OFF)
        return a4_dongle_write(dev, 0xBF01, lock);


    return A4_ERROR;
}

#define OSCAR_MRR_1000HZ       0x0001
#define OSCAR_MRR_500HZ        0x0002
#define OSCAR_MRR_250HZ        0x0004
#define OSCAR_MRR_125HZ        0x0008

int a4_mrr_set(a4_device *dev, unsigned short mrr)
{
    if (!dev)
        return A4_ERROR;

    if (mrr == OSCAR_MRR_125HZ || mrr == OSCAR_MRR_250HZ || mrr == OSCAR_MRR_500HZ || mrr == 125 || mrr == 250 || mrr == 500)
    {
        if (mrr == 125)
            mrr = OSCAR_MRR_125HZ;
        else if (mrr == 250)
            mrr = OSCAR_MRR_250HZ;
        else if (mrr == 500)
            mrr = OSCAR_MRR_500HZ;
        else if (mrr == 1000)
            mrr = OSCAR_MRR_1000HZ;

        return a4_dongle_write(dev, 0xB60F , mrr);
    }
    return A4_ERROR;
}

int a4_mrr_get(a4_device *dev)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0001, ret, 8);

    if (res != 8)
        return A4_ERROR;

    if (ret[7] == OSCAR_MRR_500HZ)
        return 500;
    else if (ret[7] == OSCAR_MRR_250HZ)
        return 250;
    else if (ret[7] == OSCAR_MRR_125HZ)
        return 125;
    else if (ret[7] == OSCAR_MRR_1000HZ)
        return 1000;

    return A4_ERROR;
}


int a4_profile_get(a4_device *dev)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0001, ret, 8);

    if (res != 8)
        return A4_ERROR;

    int profile = (ret[5] & 0xF0) >> 4;

    if (profile >= 0 && profile < 5)
        return profile;

    return A4_ERROR;
}

int a4_profile_set(a4_device *dev,int profile)
{
    if (!dev)
        return A4_ERROR;

    if (profile >= 0 && profile < 5)
        return a4_dongle_write(dev, 0xB614 , profile);

    return A4_ERROR;
}


int a4_btnmask_set(a4_device *dev,unsigned short mask)
{
    if (!dev)
        return A4_ERROR;

    return a4_dongle_write(dev, 0xB602, mask);

    return A4_ERROR;
}

int a4_btnmask_get(a4_device *dev)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0001, ret, 8);

    if (res != 8)
        return A4_ERROR;

    return ((ret[2] << 8) | ret[1]);
}

int a4_power_mouse_get(a4_device *dev)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0004, ret, 8);

    if (res != 8)
        return A4_ERROR;

    if (ret[7] & 0x80)
    {
        switch (ret[7] & 0x7F)
        {
        case 0x3F:
            return 100;
        case 0x3E:
            return 90;
        case 0x3D:
            return 80;
        case 0x3C:
            return 70;
        case 0x3B:
            return 60;
        case 0x3A:
            return 50;
        case 0x39:
            return 40;
        case 0x38:
            return 30;
        case 0x37:
            return 25;
        case 0x36:
            return 20;
        case 0x35:
            return 10;
        default:
            return 0;
        }
    }
    else
    {
        switch (ret[7])
        {
        case 0x32: case 0x33: case 0x34:
        case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3A:
        case 0x3B: case 0x3C: case 0x3D:
        case 0x3E: case 0x3F:
            return 100;
        case 0x31:
            return 90;
        case 0x30:
            return 85;
        case 0x2F:
            return 80;
        case 0x2E:
            return 75;
        case 0x2D:
            return 65;
        case 0x2C:
            return 60;
        case 0x2B:
            return 55;
        case 0x2A:
            return 30;
        case 0x29:
            return 25;
        case 0x28:
            return 20;
        case 0x27:
            return 15;
        case 0x26:
            return 5;
        default:
            return 0;
        }
    }

    return 0;
}

int a4_power_keybd_get(a4_device *dev)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];
    int res = a4_dongle_read(dev, 0xB600, 0x0004, ret, 8);

    if (res != 8)
        return A4_ERROR;

    if (ret[6] & 0x80)
    {
        switch (ret[6] & 0x7F)
        {
        case 0x3F:
            return 100;
        case 0x3E:
            return 90;
        case 0x3D:
            return 80;
        case 0x3C:
            return 70;
        case 0x3B:
            return 60;
        case 0x3A:
            return 50;
        case 0x39:
            return 40;
        case 0x38:
            return 30;
        case 0x37:
            return 25;
        case 0x36:
            return 20;
        case 0x35:
            return 10;
        default:
            return 0;
        }
    }
    else
    {
        switch (ret[6])
        {
        case 0x32: case 0x33: case 0x34:
        case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3A:
        case 0x3B: case 0x3C: case 0x3D:
        case 0x3E: case 0x3F:
            return 100;
        case 0x31:
            return 90;
        case 0x30:
            return 85;
        case 0x2F:
            return 80;
        case 0x2E:
            return 75;
        case 0x2D:
            return 65;
        case 0x2C:
            return 60;
        case 0x2B:
            return 55;
        case 0x2A:
            return 30;
        case 0x29:
            return 25;
        case 0x28:
            return 20;
        case 0x27:
            return 15;
        case 0x26:
            return 5;
        default:
            return 0;
        }
    }

    return 0;
}
