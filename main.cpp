#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#define USB_VENDOR_ID_A4         0x09DA
#define USB_PRODUCT_ID_G10_700F  0x054F



/******** INDEXES **************/
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

#define OSCAR_RF_DISTANCE_15    0x0016
#define OSCAR_RF_DISTANCE_20    0x009F

#define OSCAR_ON_BY_CLICK     0x0000
#define OSCAR_ON_BY_MOVE      0x0040
#define OSCAR_ON_BY_UNK       0x0080

#define OSCAR_MOUSE_MODE_1    0x0000
#define OSCAR_MOUSE_MODE_2    0x0001
#define OSCAR_MOUSE_MODE_3    0x0002
#define OSCAR_MOUSE_MODE_4    0x0003
#define OSCAR_MOUSE_MODE_5    0x0004

#define OSCAR_MRR_500HZ       0x0002
#define OSCAR_MRR_250HZ       0x0004
#define OSCAR_MRR_125HZ       0x0008

#define OSCAR_GET_PROFILE     0x0001
#define OSCAR_GET_MRR         0x0001
#define OSCAR_GET_CHANNEL     0x0004
#define OSCAR_GET_DISTANCE    0x0007
#define OSCAR_GET_OFFMODE     0x0007

#define OSCAR_BUTTON_RESET    0xFFFF

#define OSCAR_MOUSE_DISABLE   0x0001
#define OSCAR_MOUSE_ENABLE    0x0002

#define OSCAR_SUCCESS         0xFD



const uint8_t oscar_rf_bytes[15] = {OSCAR_RF_CHANNEL_AUTO, OSCAR_RF_CHANNEL_1,  OSCAR_RF_CHANNEL_2,  OSCAR_RF_CHANNEL_3,  OSCAR_RF_CHANNEL_4,
                                    OSCAR_RF_CHANNEL_5,    OSCAR_RF_CHANNEL_6,  OSCAR_RF_CHANNEL_7,  OSCAR_RF_CHANNEL_8,  OSCAR_RF_CHANNEL_9,
                                    OSCAR_RF_CHANNEL_10,   OSCAR_RF_CHANNEL_11, OSCAR_RF_CHANNEL_12, OSCAR_RF_CHANNEL_13, OSCAR_RF_CHANNEL_14};

const uint8_t oscar_map_channel[32] = {5, 4, 9, 4, 12, 5, 3, 13, 1, 6, 6, 7, 1, 11, 10, 12, 8, 1, 7, 13, 2, 14, 12, 5, 9, 14, 11, 14, 3, 8, 10, 2};


int set_reset_dualbtn(libusb_device_handle *dev)
{
    unsigned char ret[8];
    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                           0xB602, OSCAR_BUTTON_RESET, ret, 1, 0);
    if (res == 1)
       if (ret[0] == OSCAR_SUCCESS)
           return 0;

    return -1;
}

int set_channel_mode(libusb_device_handle *dev, int channel)
{
    if (channel >= 0 && channel < 15)
    {
        unsigned char ret[8];
        int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xBE01, oscar_rf_bytes[channel], ret, 1, 0);
        if (res == 1)
            if (ret[0] == OSCAR_SUCCESS)
                return 0;
    }
    return -1;
}

int get_channel_mode(libusb_device_handle *dev)
{
    unsigned char ret[8];
    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                           0xB600 , OSCAR_GET_CHANNEL, ret, 8, 0);
    if (res != 8)
        return -1;

    int raw_chan = ret[2] & 0x1F;
    int chan = oscar_map_channel[raw_chan];

    //0x80 bit - manual setted
    return chan | (raw_chan & 0x80);
}

int get_channel_level(libusb_device_handle *dev)
{
    unsigned char ret[8];
    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                           0xB600 , OSCAR_GET_CHANNEL, ret, 8, 0);
    if (res != 8)
        return -1;

    return ret[1];
}

int set_mouse_rf_distance(libusb_device_handle *dev, int distance)
{
    if (distance == OSCAR_RF_DISTANCE_20 || distance == OSCAR_RF_DISTANCE_15 || distance == 15 || distance == 20)
    {
        unsigned char ret[8];

        if (distance == 15)
            distance = OSCAR_RF_DISTANCE_15;
        else if (distance == 20)
            distance = OSCAR_RF_DISTANCE_20;

        int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xB61D , distance, ret, 1, 0);
        if (res == 1)
            if (ret[0] == OSCAR_SUCCESS)
                return 0;
    }
    return -1;
}

int get_mouse_rf_distance(libusb_device_handle *dev)
{
    unsigned char ret[8];
    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                           0xB600 , OSCAR_GET_DISTANCE, ret, 8, 0);
    if (res != 8)
        return -1;

    int tmp = (ret[6] == OSCAR_RF_DISTANCE_20 ? 20 : 15);

    return tmp;
}

int set_mouse_off_time_mode(libusb_device_handle *dev, int mode, int time)
{
    if (mode == OSCAR_ON_BY_CLICK || mode == OSCAR_ON_BY_MOVE || mode == OSCAR_ON_BY_UNK)
    {
        unsigned char ret[8];

        //same with oscar editor
        if (time > 0x0a)
            time=0x0a;
        else if (time < 0x03)
            time=0x03;

        uint16_t setting = (mode & 0xC0) | (time & 0x3F);

        int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xB61A , setting, ret, 1, 0);
        if (res == 1)
            if (ret[0] == OSCAR_SUCCESS)
                return 0;
    }
    return -1;
}

int get_mouse_off_time_mode(libusb_device_handle *dev)
{
    unsigned char ret[8];
    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                           0xB600 , OSCAR_GET_OFFMODE, ret, 8, 0);
    if (res != 8)
        return -1;

    return ret[7];
}

int set_mouse_current_mrr(libusb_device_handle *dev, int mrr)
{
    if (mrr == OSCAR_MRR_125HZ || mrr == OSCAR_MRR_250HZ || mrr == OSCAR_MRR_500HZ || mrr == 125 || mrr == 250 || mrr == 500)
    {
        unsigned char ret[8];

        if (mrr == 125)
            mrr = OSCAR_MRR_125HZ;
        else if (mrr == 250)
            mrr = OSCAR_MRR_250HZ;
        else if (mrr == 500)
            mrr = OSCAR_MRR_500HZ;

        int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xB60F , mrr, ret, 1, 0);
        if (res == 1)
            if (ret[0] == OSCAR_SUCCESS)
                return 0;
    }
    return -1;
}

int get_mouse_current_mrr(libusb_device_handle *dev)
{
    unsigned char ret[8];
    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                           0xB600 , OSCAR_GET_MRR, ret, 8, 0);
    if (res != 8)
        return -1;

    if (ret[7] == OSCAR_MRR_500HZ)
        return 500;
    else if (ret[7] == OSCAR_MRR_250HZ)
        return 250;
    else if (ret[7] == OSCAR_MRR_125HZ)
        return 125;

    return -1;
}

int get_mouse_current_profile(libusb_device_handle *dev)
{
    unsigned char ret[8];
    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                           0xB600 , OSCAR_GET_PROFILE, ret, 8, 0);
    if (res != 8)
        return -1;

    int profile = (ret[5] & 0xF0) >> 4;

    if (profile >= 0 && profile < 5)
        return profile;

    return -1;
}

int set_mouse_current_profile(libusb_device_handle *dev,int profile)
{
    if (profile >= 0 && profile < 5)
    {
        unsigned char ret[8];

        int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xB614 , profile, ret, 1, 0);
        if (res == 1)
            if (ret[0] == OSCAR_SUCCESS)
                return 0;
    }
    return -1;
}


int set_paging(libusb_device_handle *dev,unsigned short paging)
{
    unsigned char ret[8];

    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xBE0A , paging, ret, 1, 0);
    if (res == 1)
        if (ret[0] == OSCAR_SUCCESS)
            return 0;

    return -1;
}

int set_disable_enable_mouse(libusb_device_handle *dev,unsigned short disable)
{
    unsigned char ret[8];

    if (disable == OSCAR_MOUSE_DISABLE || disable == OSCAR_MOUSE_ENABLE)
    {
        int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xBF01 , disable, ret, 1, 0);
        if (res == 1)
            if (ret[0] == OSCAR_SUCCESS)
                return 0;
    }

    return -1;
}

int set_page(libusb_device_handle *dev,unsigned short page)
{
    unsigned char ret[8];

    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xB502 , page, ret, 1, 0);
    if (res == 1)
        if (ret[0] == OSCAR_SUCCESS)
            return 0;

    return -1;
}

int write_bytes_to_mouse(libusb_device_handle *dev,unsigned short addr, unsigned short word)
{
    unsigned char ret[8];



    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
                                               0xB504 , page, ret, 1, 0);
    if (res == 1)
        if (ret[0] == OSCAR_SUCCESS)
            return 0;

    return -1;
}


struct macros_instr
{
    char   *string;
    uint16_t code;
};

macros_instr macro_no_params[] = {{"wheelup",0x2BFB},{"wheeldown",0x2BFC},{"leftdown",0x2501},{"leftup",0x2401},{"middledown",0x2504},
{"middleup",0x2404},{"rightdown",0x2502},{"rightup",0x2402},{"button4down",0x2508},{"button4up",0x2408},
{"button5down",0x2510},{"button5up",0x2410},{"rightwheeldown",0x2BEC},{"rightwheelup",0x2BED},{"leftwheeldown",0x2BEE},
{"leftwheelup",0x2BEF},{"swingr",0x2BF0},{"swingl",0x2BF1},{"wheelup",0x2BFB},{"wheeldown",0x2BFC},
{"stop",0x2BFD},{"null",0x2BFE},{"macroending",0x2BFF}};


macros_instr macro_w_params[] = {{"delay",0x2800},{"delays",0x2C00},{"keydown",0x2100},{"keyup",0x2000},{"mover",0x4000},{"moverx",0x0000},{"movery",0x1000}};




int main()
{

	libusb_context *ctx = NULL;

	int r = libusb_init(&ctx);

	if(r < 0)
	{
		printf("Init Error\n");
        return 1;
	}

	libusb_set_debug(ctx, 3);

    libusb_device_handle* dvs = libusb_open_device_with_vid_pid(ctx,USB_VENDOR_ID_A4,USB_PRODUCT_ID_G10_700F);

    if (dvs)
    {

        //set_mouse_current_profile(dvs,0);

        while (true)
        {

            int chan = get_mouse_current_profile(dvs);

            printf("%d\n",chan);

            sleep(1);
        }
        libusb_close(dvs);
    }

    libusb_exit(ctx);
    return 0;
}
