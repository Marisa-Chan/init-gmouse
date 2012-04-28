#ifndef CONTROL_A4_H_INCLUDED
#define CONTROL_A4_H_INCLUDED
#include "system_a4.h"

int a4_rf_set_channel(a4_device *dev, int channel);

struct a4_rf_channel
{
    int channel;
    int type;
};

#define A4_CHAN_AUTO    0
#define A4_CHAN_MANUAL  1

a4_rf_channel a4_rf_get_channel(a4_device *dev);

int a4_rf_get_signal_level(a4_device *dev);
int a4_rf_set_distance(a4_device *dev, int distance);
int a4_rf_get_distance(a4_device *dev);

#define A4_WAKE_BY_CLICK     0x0000
#define A4_WAKE_BY_MOVE      0x0040
//#define A4_WAKE_BY_UNK       0x0080

struct a4_wake_mode
{
    int time;
    int type;
};

int a4_wake_set_mode(a4_device *dev, a4_wake_mode mode);
a4_wake_mode a4_wake_get_mode(a4_device *dev);

#define A4_LOCK_ON   0x0001
#define A4_LOCK_OFF  0x0002

int a4_lock_mouse(a4_device *dev, unsigned short lock);


int a4_mrr_set(a4_device *dev, unsigned short mrr);
int a4_mrr_get(a4_device *dev);

int a4_profile_get(a4_device *dev);
int a4_profile_set(a4_device *dev,int profile);

int a4_btnmask_set(a4_device *dev,unsigned short mask);
int a4_btnmask_get(a4_device *dev);

int a4_power_mouse_get(a4_device *dev);
int a4_power_keybd_get(a4_device *dev);

#endif // CONROL_A4_H_INCLUDED
