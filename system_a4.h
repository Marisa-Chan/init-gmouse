#include <libusb-1.0/libusb.h>

#ifndef SYSTEM_A4_H_INCLUDED
#define SYSTEM_A4_H_INCLUDED

#define A4_ERROR    -1
#define A4_SUCCESS   0


struct a4_device
{
    libusb_device_handle *dev;
    libusb_context *ctx;
};

a4_device *a4_open_device();
void a4_close_device(a4_device *dev);

int a4_dongle_read(a4_device *dev, unsigned short address, unsigned short pin, void *buffer, int bf_size);
int a4_dongle_write(a4_device *dev, unsigned short addr_pin, unsigned short word);
int a4_dump(a4_device *dev, const char *file);

#endif // SYSTEM_A4_H_INCLUDED
