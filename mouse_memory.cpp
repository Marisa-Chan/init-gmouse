#include "system_a4.h"

//erase 0x100 block
int a4_mem_erase_block(a4_device *dev, unsigned short page)
{
    if (!dev)
        return A4_ERROR;

    if ((page & 0x3FFF) < 0x1f00)
        return A4_ERROR;

    return a4_dongle_write(dev, 0xB502, page);
}

//works only with erased block
int a4_mem_mov_block(a4_device *dev, unsigned short addr)
{
    if (!dev)
        return A4_ERROR;

    if ((addr & 0x3FFF) < 0x1f00)
        return A4_ERROR;

    return a4_dongle_write(dev, 0xB500, addr | 0x8000);
}

//works only with erased block
int a4_mem_mov_word(a4_device *dev, unsigned short addr)
{
    if (!dev)
        return A4_ERROR;

    if ((addr & 0x3FFF) < 0x1f00)
        return A4_ERROR;

    return a4_dongle_write(dev, 0xB500, addr);
}

int a4_mem_write_word(a4_device *dev, unsigned short offset, unsigned short word)
{
    if (!dev)
        return A4_ERROR;

    unsigned short realaddr = offset * 2;

    if (realaddr >= 0x40 || !dev)
        return A4_ERROR;

    unsigned short packet = (realaddr & 0xFF) | ((word & 0xFF) << 8);

    int res = a4_dongle_write(dev, 0xB504, packet);

    if (res == A4_ERROR)
        return A4_ERROR;

    realaddr++;

    if (realaddr >= 0x40)
        return A4_ERROR;

    packet = (realaddr & 0xFF) | (word & 0xFF00);

    return a4_dongle_write(dev, 0xB504, packet);
}

unsigned short a4_mem_read_word(a4_device *dev, unsigned short addr)
{
    if (!dev)
        return A4_ERROR;

    unsigned char ret[8];

    int res = a4_dongle_read(dev, 0xB501, addr, ret, 2);

    if (res != 2)
        return 0xFFFF;

    unsigned short val = ret[0] | (ret[1] << 8);
    return val;
}

int a4_mem_read_block(a4_device *dev, unsigned short addr, unsigned short words_sz, void *buf)
{
    if (!dev)
        return A4_ERROR;

    unsigned short *sbuf = (unsigned short *)buf;

    int big   = words_sz / 4;
    int small = words_sz % 4;

    for(int i = 0; i < big; i++)
    {
        int res = a4_dongle_read(dev, 0xB501, addr + (i*4), sbuf + (i*4), 8);

        if (res != 8)
            return A4_ERROR;
    }

    for(int i = 0; i < small; i++)
    {
        int res = a4_dongle_read(dev, 0xB501, addr + (big * 4) + i, sbuf + (big * 4) + i, 2);

        if (res != 2)
            return A4_ERROR;
    }

    return A4_SUCCESS;
}
