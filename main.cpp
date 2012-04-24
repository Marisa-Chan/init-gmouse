#include <stdio.h>
#include <stdlib.h>
#include "system_a4.h"
#include "pairing.h"

#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include <string.h>
#include "./mouse_memory.h"


//int set_unknown(libusb_device_handle *dev,unsigned short paging)
//{
//    unsigned char ret[8];
//
//    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
//                                               0xBE0A , paging, ret, 1, 0);
//    if (res == 1)
//        if (ret[0] == OSCAR_SUCCESS)
//            return 0;
//
//    return -1;
//}
//
//
//
//
//
//int set_B60E(libusb_device_handle *dev,unsigned short val)
//{
//    unsigned char ret[8];
//
//    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
//                                               0xB60E , val, ret, 1, 0);
//    if (res == 1)
//        if (ret[0] == OSCAR_SUCCESS)
//            return 0;
//
//    return -1;
//}
//
//int set_B613(libusb_device_handle *dev,unsigned short val)
//{
//    unsigned char ret[8];
//
//    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
//                                               0xB613 , val, ret, 1, 0);
//    if (res == 1)
//        if (ret[0] == OSCAR_SUCCESS)
//            return 0;
//
//    return -1;
//}
//
//
//struct t_params
//{
//    unsigned short write_addr;
//    unsigned short checksum;
//    unsigned short max_addr;
//};
//
//t_params get_3_params(libusb_device_handle *dev)
//{
//    t_params result;
//
//    result.write_addr = 0x2000;
//
//    unsigned char ret[8];
//
//    int res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
//                                               0xB600 , 0x03, ret, 8, 0);
//
//    if (res == 8)
//    {
//        if (ret[3] != 0x0)
//            result.write_addr = (ret[3] << 8)| ret[4] ;
//    }
//
//
//    res = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN, LIBUSB_REQUEST_GET_DESCRIPTOR,
//                                               0xB600 , 0x02, ret, 8, 0);
//
//    if (res == 8)
//    {
//        result.max_addr = (ret[1] << 6) + 0xFF00 ;
//    }
//
//
//    return result;
//}
//


/*void write_config_to_mouse(libusb_device_handle *dev,unsigned short *buffer, int number)
{

    int to_write = 0xFFFFFF80 & (number+0x7F);
    unsigned short *buf = (unsigned short *)malloc(to_write *2);
    memset(buf,0xFF,to_write*2);

    memcpy(buf,buffer,number*2);

    unsigned short rate = buf[3];
    buf[3] = 0xFFFF;
                                                                                                                                                                                 printf("ERROR, write config is not complated, this may kill your device.");
     //                                                                                                                                                                                     exit(-1);
//

    t_params params1 = get_3_params(dev);

//    if (params1.max_addr)

    set_unknown(dev,0x80); //WORKS_OK

    t_params params = get_3_params(dev);

    //printf("0x%x\n\n\n",params.write_addr);
   // printf("0x%x\n\n\n",params.max_addr);
    if (params.write_addr < 0x1f00)
        exit(-1);


    set_disable_enable_mouse(dev,OSCAR_MOUSE_DISABLE); //WORKS_OK

    sleep(1);

    for (int i=0; i < to_write; i++)
    {
        if ((i & 0xFFFFFF80) == i)
        {
            printf("set page 0x%x\n",params.write_addr + i);
            write_FFFF_0x80_block(dev,params.write_addr + i);
        }

        unsigned short j = i & 0x1F;

        printf("write bytes at 0x%x 0x%x ",j,buf[i]);
        printf("%d \n",write_bytes_to_mouse(dev,j,buf[i]));

        if (j == 0x1F)
        {
            printf("mouse_move_block to 0x%x\n",((params.write_addr + i) & 0xFFFFFFE0) | 0x8000 );
            mouse_move_block(dev, ((params.write_addr + i) & 0xFFFFFFE0) | 0x8000 );
        }

        //for (int z=0; z<)
    }

    write_bytes_to_mouse(dev,0,rate);
    mouse_move_block(dev, params.write_addr + 3);
    //check

    set_B60E(dev,01); //
    set_B613(dev,0x10);
    //set_mouse_current_mrr(dev,OSCAR_MRR_125HZ);//b60f //set_current MRR

    set_unknown(dev,0x0); //WORKS_OK

    set_disable_enable_mouse(dev,OSCAR_MOUSE_ENABLE); //WORKS_OK
  //  set_mouse_current_profile(dev,OSCAR_MOUSE_MODE_2);
}
*/


struct macros_instr
{
    char   *string;
    uint16_t code;
};

macros_instr macro_no_params[] = {{"wheelup",0x2BFB},{"wheeldown",0x2BFC},{"leftdown",0x2501},{"leftup",0x2401},{"middledown",0x2504},
    {"middleup",0x2404},{"rightdown",0x2502},{"rightup",0x2402},{"button4down",0x2508},{"button4up",0x2408},
    {"button5down",0x2510},{"button5up",0x2410},{"rightwheeldown",0x2BEC},{"rightwheelup",0x2BED},{"leftwheeldown",0x2BEE},
    {"leftwheelup",0x2BEF},{"swingr",0x2BF0},{"swingl",0x2BF1},{"wheelup",0x2BFB},{"wheeldown",0x2BFC},
    {"stop",0x2BFD},{"null",0x2BFE},{"macroending",0x2BFF}
};


macros_instr macro_w_params[] = {{"delay",0x2800},{"delays",0x2C00},{"keydown",0x2100},{"keyup",0x2000},{"mover",0x4000},{"moverx",0x0000},{"movery",0x1000}};



int main()
{

    a4_device * dvs = a4_open_device();

    if (dvs)
    {

        a4_pair_set_find_mode(dvs,A4_PAIR_FIND_ON);

        while(true)
        {
            a4_pair_device tmp = a4_pair_get_new_device(dvs);

            if (tmp.type != A4_PAIR_NONE)
            {
                a4_pair_add_new_device(dvs,tmp);

                break;
            }
            sleep(1);
        }

        a4_pair_set_find_mode(dvs,A4_PAIR_FIND_OFF);


        a4_pair_devlist tt =  a4_pair_get_list_mouse(dvs);
        a4_pair_devlist tt2 =  a4_pair_get_list_keyboard(dvs);

        printf("Mouses:\n");
        for (int i=0; i<5; i++)
        {
            if (tt.ID[i] != A4_PAIR_NONE)
            {
                if (tt.disabled[i] == A4_PAIR_STATE_ENABLE)
                    printf("+ ");
                else
                    printf("- ");
                printf("%.8x\n",tt.ID[i]);
            }
        }
        printf("Keyboards:\n");
        for (int i=0; i<5; i++)
        {
            if (tt2.ID[i] != A4_PAIR_NONE)
            {
                if (tt2.disabled[i] == A4_PAIR_STATE_ENABLE)
                    printf("+ ");
                else
                    printf("- ");
                printf("%.8x\n",tt2.ID[i]);
            }
        }

        //a4_pair_del_device_by_id(dvs,0x000172f7);

    }

    a4_close_device(dvs);
    return 0;
}
