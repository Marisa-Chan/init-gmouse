#ifndef CONTROL_A4_H_INCLUDED
#define CONTROL_A4_H_INCLUDED
#include "system_a4.h"

int a4_rf_set_channel(a4_device *dev, int channel);
int a4_rf_get_channel(a4_device *dev);
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

#endif // CONROL_A4_H_INCLUDED
