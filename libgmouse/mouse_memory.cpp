#include "system_a4.h"
#include <string.h>
#include <stdio.h>

//erase 0x100 block
int a4_mem_erase_block(a4_device *dev, unsigned short page)
{
    if (!dev)
        return A4_ERROR;

    if ((page & 0x3FFF) < 0x1f00 && page >= 0x4000)
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

int a4_mem_read_block(a4_device *dev, unsigned short addr, unsigned short words_sz, void *buf,void (*progress)(int))
{
    if (!dev)
        return A4_ERROR;

    unsigned short *sbuf = (unsigned short *)buf;

    int big   = words_sz / 4;
    int small = words_sz % 4;

    float pone = 1.0;
    if (big > 0)
        pone = 100.0 / big;

    float ptwo = 0;

    for(int i = 0; i < big; i++)
    {
        int res = a4_dongle_read(dev, 0xB501, addr + (i*4), sbuf + (i*4), 8);

        if (res != 8)
            return A4_ERROR;

        ptwo += pone;
        if (progress != NULL)
            progress(ptwo);
    }

    for(int i = 0; i < small; i++)
    {
        int res = a4_dongle_read(dev, 0xB501, addr + (big * 4) + i, sbuf + (big * 4) + i, 2);

        if (res != 2)
            return A4_ERROR;
    }

    if ( progress != NULL)
        progress(100);

    return A4_SUCCESS;
}

int a4_mem_write_block(a4_device *dev, unsigned short addr, unsigned short words_sz, void *inbuf, void (*progress)(int))
{
    if (!dev)
        return A4_ERROR;

    int to_write = 0xFFFFFF80 & (words_sz+0x7F);
    unsigned short *buf = (unsigned short *)malloc(to_write *2);
    memset(buf,0xFF,to_write*2);

    memcpy(buf,inbuf,words_sz*2);

    int error = A4_SUCCESS;

    float pone = 1.0;
    if (to_write > 0)
        pone = 100.0 / to_write;

    float ptwo = 0;

    for (int i=0; i < to_write && error == A4_SUCCESS; i++)
    {
        unsigned int k = i;
        if ((k & 0xFFFFFF80) == k)
        {
            if ( progress != NULL)
                progress(ptwo);
            //printf("set page 0x%x\n",addr + i);
            error = a4_mem_erase_block(dev,addr + k);
        }

        unsigned short j = i & 0x1F;

        //printf("write at pos %.2x word 0x%.4x\n",j,buf[i]);
        error = a4_mem_write_word(dev,j,buf[i]);

        if (j == 0x1F)
        {
            //printf("mouse_move_block to 0x%x\n",((addr + i) & 0xFFFFFFE0) | 0x8000 );
            error = a4_mem_mov_block(dev, ((addr + i) & 0xFFFFFFE0) | 0x8000 );
        }

        ptwo += pone;
    }

    if ( progress != NULL)
        progress(100);

    return error;
}
