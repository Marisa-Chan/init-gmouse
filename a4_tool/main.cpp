#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "system_a4.h"
#include "pairing.h"
#include "control_a4.h"

#define LITLE_SLEEP   250000


typedef int (*func)(a4_device *dev, int argc, char *argv[]);

int pair_func(a4_device *dev, int argc, char *argv[]);
int channel_func(a4_device *dev, int argc, char *argv[]);
int distance_func(a4_device *dev, int argc, char *argv[]);
int wake_func(a4_device *dev, int argc, char *argv[]);
int mrr_func(a4_device *dev, int argc, char *argv[]);


struct funcs
{
    const char	*func_name;
    const char	*arg_desc;
    const char	*misc;
    func	    funct;
};


static funcs functions[] =
{
    {
        "pair",
        "list | new | del <ID> | disable <ID> | enable <ID>",
        NULL,
        pair_func
    },
    {
        "channel",
        "get | set chan",
        "chan - [1-14, 0-auto]",
        channel_func
    },
    {
        "distance",
        "get | set (15 | 20)",
        NULL,
        distance_func
    },
    {
        "sleep",
        "get | set wake_by time",
        "wake_by=[click | move] time=3..10min",
        wake_func
    },
    {
        "mrr",
        "get | set (125 | 250 | 500)",
        NULL,
        mrr_func
    },
    {
        NULL,
        NULL,
        NULL,
        NULL
    }
};


int mrr_func(a4_device *dev, int argc, char *argv[])
{
    if (argc > 0)
    {
        if (strcmp(argv[0],"get") == 0)
        {
            int mrr = a4_mrr_get(dev);

            if (mrr == A4_ERROR)
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }

            printf("Current MRR: %dHz\n", mrr);

        }
        else if (strcmp(argv[0],"set") == 0)
        {
            if (argc < 2)
            {
                fprintf(stderr, "No mrr specified\n");
                return EXIT_FAILURE;
            }

            int mrr = 0;

            mrr = -1;
            sscanf(argv[1],"%d",&mrr);



            if (mrr != 125 && mrr != 250 && mrr != 500)
            {
                fprintf(stderr, "Invalid mrr \"%s\"\n", argv[1]);
                return EXIT_FAILURE;
            }

            printf("Setting mrr to %dHz...\n",mrr);

            usleep(LITLE_SLEEP);

            if (a4_mrr_set(dev, mrr) == A4_SUCCESS)
                printf("Success.\n");
            else
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            fprintf(stderr, "Unknown option \"%s\"\n",argv[0]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "No options specified\n");
        return EXIT_FAILURE;
    }

    return 0;
}

int wake_func(a4_device *dev, int argc, char *argv[])
{
    if (argc > 0)
    {
        if (strcmp(argv[0],"get") == 0)
        {
            a4_wake_mode mod = a4_wake_get_mode(dev);

            if (mod.type == A4_ERROR)
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }

            printf("Sleep after: %d min, Wake by:", mod.time);

            if (mod.type == A4_WAKE_BY_CLICK)
                printf(" click\n");
            else
                printf(" move\n");
        }
        else if (strcmp(argv[0],"set") == 0)
        {
            if (argc < 3)
            {
                fprintf(stderr, "Options: <wake_by> <time>\n");
                return EXIT_FAILURE;
            }

            a4_wake_mode mod;

            if (strcmp(argv[1],"click") == 0)
                mod.type = A4_WAKE_BY_CLICK;
            else if (strcmp(argv[1],"move") == 0)
                mod.type = A4_WAKE_BY_MOVE;
            else
            {
                fprintf(stderr, "Incorrect value: \"%s\"\n",argv[1]);
                return EXIT_FAILURE;
            }


            int time = 0;

            sscanf(argv[2],"%d",&time);

            if (time < 3 || time > 10)
            {
                fprintf(stderr, "Invalid time value: \"%s\"\n", argv[2]);
                return EXIT_FAILURE;
            }

            printf("Setting sleep to %d min and wake by %s...\n",time,argv[1]);

            usleep(LITLE_SLEEP);

            mod.time = time;

            if (a4_wake_set_mode(dev, mod) == A4_SUCCESS)
                printf("Success.\n");
            else
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            fprintf(stderr, "Unknown option \"%s\"\n",argv[0]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "No options specified\n");
        return EXIT_FAILURE;
    }

    return 0;
}


int distance_func(a4_device *dev, int argc, char *argv[])
{
    if (argc > 0)
    {
        if (strcmp(argv[0],"get") == 0)
        {
            int dist = a4_rf_get_distance(dev);

            if (dist == A4_ERROR)
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }

            printf("Operate distance: %dm\n", dist);

        }
        else if (strcmp(argv[0],"set") == 0)
        {
            if (argc < 2)
            {
                fprintf(stderr, "No distance specified\n");
                return EXIT_FAILURE;
            }

            int idx = 0;

            idx = -1;
            sscanf(argv[1],"%d",&idx);



            if (idx != 15 && idx != 20)
            {
                fprintf(stderr, "Invalid distance \"%s\"\n", argv[1]);
                return EXIT_FAILURE;
            }

            printf("Setting distance to %dm...\n",idx);

            usleep(LITLE_SLEEP);

            if (a4_rf_set_distance(dev, idx) == A4_SUCCESS)
                printf("Success.\n");
            else
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            fprintf(stderr, "Unknown option \"%s\"\n",argv[0]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "No options specified\n");
        return EXIT_FAILURE;
    }

    return 0;
}

int channel_func(a4_device *dev, int argc, char *argv[])
{
    if (argc > 0)
    {
        if (strcmp(argv[0],"get") == 0)
        {
            a4_rf_channel chn = a4_rf_get_channel(dev);

            if (chn.type == A4_ERROR)
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }

            printf("Channel: %d", chn.channel);

            if (chn.type == A4_CHAN_MANUAL)
                printf(" manual\n");
            else
                printf(" auto\n");
        }
        else if (strcmp(argv[0],"set") == 0)
        {
            if (argc < 2)
            {
                fprintf(stderr, "No channel number specified\n");
                return EXIT_FAILURE;
            }

            int idx = 0;

            if (strcmp(argv[1],"auto") == 0)
                idx = 0;
            else
            {
                idx = -1;
                sscanf(argv[1],"%d",&idx);
            }


            if (idx < 0 || idx > 14)
            {
                fprintf(stderr, "Invalid channel \"%s\"\n", argv[1]);
                return EXIT_FAILURE;
            }

            if (idx != 0)
                printf("Setting %d channel...\n",idx);
            else
                printf("Setting auto channel...\n");

            usleep(LITLE_SLEEP);

            if (a4_rf_set_channel(dev, idx) == A4_SUCCESS)
                printf("Success.\n");
            else
            {
                fprintf(stderr, "IO Error\n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            fprintf(stderr, "Unknown option \"%s\"\n",argv[0]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "No options specified\n");
        return EXIT_FAILURE;
    }

    return 0;
}

int pair_func(a4_device *dev, int argc, char *argv[])
{
    if (argc > 0)
    {
        if (strcmp(argv[0],"list") == 0)
        {
            printf("Device list:\n\n");
            a4_pair_devlist lst = a4_pair_get_list_mouse(dev);
            printf("Mouse:\n");
            for (int i=0; i<5; i++)
                if (lst.ID[i] != A4_PAIR_NONE)
                {
                    printf("\t%.8X",lst.ID[i]);
                    if (lst.disabled[i])
                        printf(" disabled\n");
                    else
                        printf("\n");
                }
            lst = a4_pair_get_list_keyboard(dev);
            printf("\nKeyboard:\n");
            for (int i=0; i<5; i++)
                if (lst.ID[i] != A4_PAIR_NONE)
                {
                    printf("\t%.8X",lst.ID[i]);
                    if (lst.disabled[i])
                        printf(" disabled\n");
                    else
                        printf("\n");
                }
        }
        else if (strcmp(argv[0],"new") == 0)
        {
            printf("Adding new device\n\nSwitch power on new device to OFF(if it in ON), and switch it ON.\n");
            printf("Waiting for new device...\n");

            int count = 0;

            a4_pair_set_find_mode(dev,A4_PAIR_FIND_ON);

            int result = A4_PAIR_MOD_OK;

            while(count < 60)
            {
                a4_pair_device device = a4_pair_get_new_device(dev);

                if (device.type != A4_PAIR_NONE)
                {
                    a4_pair_set_find_mode(dev,A4_PAIR_FIND_OFF);

                    if (device.type == A4_PAIR_MOUSE)
                    {
                        printf("Got mouse with ID %.8X\n",device.ID);
                        printf("\nAdding to dongle...\n");

                        result = a4_pair_add_new_device(dev, device);
                    }
                    else if (device.type == A4_PAIR_KBD)
                    {
                        printf("Got keyboard with ID %.8X\n",device.ID);
                        printf("\nAdding to dongle...\n");

                        result = a4_pair_add_new_device(dev, device);
                    }

                    break;
                }

                count++;
                sleep(1);
            }

            if (count >= 60)
            {
                a4_pair_set_find_mode(dev,A4_PAIR_FIND_OFF);
                printf("Time out.\n");
                return EXIT_FAILURE;
            }

            printf("%s.\n",a4_pair_get_error_string(result));

            if (result != A4_PAIR_MOD_OK)
                return EXIT_FAILURE;
        }
        else if (strcmp(argv[0],"del") == 0)
        {
            if (argc < 2)
            {
                fprintf(stderr, "No ID specified\n");
                return EXIT_FAILURE;
            }

            unsigned int ID = 0;

            int res = sscanf(argv[1],"%x",&ID);
            if (res != 1 || ID == 0)
            {
                fprintf(stderr, "Invalid ID\n");
                return EXIT_FAILURE;
            }

            printf("Deleting device with ID: %.8X ...\n",ID);
            res = a4_pair_del_device_by_id(dev, ID);

            printf("%s.\n",a4_pair_get_error_string(res));

            if (res != A4_PAIR_MOD_OK)
                return EXIT_FAILURE;
        }
        else if (strcmp(argv[0],"disable") == 0)
        {
            if (argc < 2)
            {
                fprintf(stderr, "No ID specified\n");
                return EXIT_FAILURE;
            }

            unsigned int ID = 0;

            int res = sscanf(argv[1],"%x",&ID);
            if (res != 1 || ID == 0)
            {
                fprintf(stderr, "Invalid ID\n");
                return EXIT_FAILURE;
            }

            printf("Disabling device with ID: %.8X ...\n",ID);
            res = a4_pair_switch_device_by_id(dev, ID, A4_PAIR_STATE_DISABLE);

            printf("%s.\n",a4_pair_get_error_string(res));

            if (res != A4_PAIR_MOD_OK)
                return EXIT_FAILURE;
        }
        else if (strcmp(argv[0],"enable") == 0)
        {
            if (argc < 2)
            {
                fprintf(stderr, "No ID specified\n");
                return EXIT_FAILURE;
            }

            unsigned int ID = 0;

            int res = sscanf(argv[1],"%x",&ID);
            if (res != 1 || ID == 0)
            {
                fprintf(stderr, "Invalid ID\n");
                return EXIT_FAILURE;
            }

            printf("Enabling device with ID: %.8X ...\n",ID);
            res = a4_pair_switch_device_by_id(dev, ID, A4_PAIR_STATE_ENABLE);

            printf("%s.\n",a4_pair_get_error_string(res));

            if (res != A4_PAIR_MOD_OK)
                return EXIT_FAILURE;
        }
        else
        {
            fprintf(stderr, "Unknown option \"%s\"\n",argv[0]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "No options specified\n");
        return EXIT_FAILURE;
    }

    return 0;
}





static void help_list_commands(void)
{
    funcs *pfnc = functions;

    fprintf(stderr, "usage :\n");

    while (pfnc->funct)
    {
        if (!pfnc->misc)
            fprintf(stderr, "a4_tool %s <%s>\n", pfnc->func_name, pfnc->arg_desc);
        else
            fprintf(stderr, "a4_tool %s <%s>// %s\n", pfnc->func_name, pfnc->arg_desc,pfnc->misc);
        pfnc++;
    }
}



int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        help_list_commands();
        return EXIT_FAILURE;
    }

    funcs *pfnc = functions;

    while (pfnc->funct)
    {
        if (strcmp(pfnc->func_name, argv[1]) == 0 )
        {

            a4_device * dev = a4_open_device();

            if (dev)
            {
                return pfnc->funct(dev,argc-2,argv+2);
                a4_close_device(dev);
            }
            else
                fprintf(stderr, "No a4 dongle.\n");

            return EXIT_FAILURE;
        }

        pfnc++;
    }

    help_list_commands();

    return EXIT_FAILURE;
}
