#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 32

void *parse_cmd(void *dst_buf, int len, const char *cmd_line, unsigned short offset)
{
	if (len < 2)
	{
		printf("out buf too small\n");
		return dst_buf;
	}
		

	char buf[256];
	strcpy(buf, cmd_line);

	for(unsigned int i = 0; i < strlen(buf); i++)
		buf[i] = tolower(buf[i]);

	char *tokens[MAX_TOKENS];
	int token_cnt = 0;

	char * pch = strtok(buf, " ");
	while (pch != NULL)
	{
		if (token_cnt < MAX_TOKENS)
		{
			tokens[token_cnt] = pch;
			token_cnt++;
		}

		pch = strtok (NULL, " ");
	}

	if (strcmp(tokens[0], "keydown") == 0)
	{
		if (token_cnt < 2)
			return dst_buf;
		int keycode = atoi(tokens[1]);
		unsigned short out_code = 0;
		switch (keycode)
		{
		case 264:
			out_code = 0x2301;
			break;
		case 265:
			out_code = 0x2302;
			break;
		case 266:
			out_code = 0x2304;
			break;
		case 267:
			out_code = 0x2308;
			break;
		case 268:
			out_code = 0x2310;
			break;
		case 269:
			out_code = 0x2320;
			break;
		case 270:
			out_code = 0x2340;
			break;
		case 271:
			out_code = 0x2380;
			break;
		case 272:
			out_code = 0x34B6;
			break;
		case 273:
			out_code = 0x34B5;
			break;
		case 274:
			out_code = 0x34B7;
			break;
		case 275:
			out_code = 0x34CD;
			break;
		case 276:
			out_code = 0x34E2;
			break;
		case 277:
			out_code = 0x34E9;
			break;
		case 278:
			out_code = 0x34EA;
			break;
		case 279:
			out_code = 0x3592;
			break;
		case 280:
			out_code = 0x3623;
			break;
		case 281:
			out_code = 0x358A;
			break;
		case 282:
			out_code = 0x3594;
			break;
		case 283:
			out_code = 0x3621;
			break;
		case 284:
			out_code = 0x362A;
			break;
		case 285:
			out_code = 0x2BFE;
			break;
		case 286:
			out_code = 0x359C;
			break;
		case 287:
			out_code = 0x3981;
			break;
		case 299:
			out_code = 0x3624;
			break;
		case 300:
			out_code = 0x3625;
			break;
		case 301:
			out_code = 0x3626;
			break;
		case 302:
			out_code = 0x3627;
			break;
		case 303:
			out_code = 0x3982;
			break;
		case 304:
			out_code = 0x3583;
			break;
		case 308:
			out_code = 0x2BF2;
			break;
		default:
		    out_code = (keycode & 0xFF) | 0x2100;
            break;
		}
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = out_code;
        dst_buf = buf + 1;
	}
	else if (strcmp(tokens[0], "keyup") == 0)
	{
	    if (token_cnt < 2)
			return dst_buf;
		int keycode = atoi(tokens[1]);
		unsigned short out_code = 0;
		switch (keycode)
		{
        case 264:
          out_code = 0x2201u;
          break;
        case 265:
          out_code = 0x2202u;
          break;
        case 266:
          out_code = 0x2204u;
          break;
        case 267:
          out_code = 0x2208u;
          break;
        case 268:
          out_code = 0x2210u;
          break;
        case 269:
          out_code = 0x2220u;
          break;
        case 270:
          out_code = 0x2240u;
          break;
        case 271:
          out_code = 0x2280u;
          break;
        case 272:
          out_code = 0x30B6u;
          break;
        case 273:
          out_code = 0x30B5u;
          break;
        case 274:
          out_code = 0x30B7u;
          break;
        case 275:
          out_code = 0x30CDu;
          break;
        case 276:
          out_code = 0x30E2u;
          break;
        case 277:
          out_code = 0x30E9u;
          break;
        case 278:
          out_code = 0x30EAu;
          break;
        case 279:
          out_code = 0x3192u;
          break;
        case 280:
          out_code = 0x3223u;
          break;
        case 281:
          out_code = 0x318Au;
          break;
        case 282:
          out_code = 0x3194u;
          break;
        case 283:
          out_code = 0x3221u;
          break;
        case 284:
          out_code = 0x322Au;
          break;
        case 285:
          out_code = 0x2BFEu;
          break;
        case 286:
          out_code = 0x319Cu;
          break;
        case 287:
          out_code = 0x3881u;
          break;
        case 299:
          out_code = 0x3224u;
          break;
        case 300:
          out_code = 0x3225u;
          break;
        case 301:
          out_code = 0x3226u;
          break;
        case 302:
          out_code = 0x3227u;
          break;
        case 303:
          out_code = 0x3882u;
          break;
        case 304:
          out_code = 0x3183u;
          break;
        case 308:
          out_code = 0x2BF3u;
          break;
        default:
          out_code = (keycode & 0xFF) | 0x2000;
          break;
		}
		unsigned short *buf = (unsigned short *)dst_buf;
        *buf = out_code;
        dst_buf = buf + 1;
	}
	else if (strcmp(tokens[0], "mover") == 0)
    {
        if (token_cnt < 3)
	{
		printf("mover tokens %d\n", token_cnt);
		return dst_buf;
	}

        int mov_x = atoi(tokens[1]);
        int mov_y = atoi(tokens[2]);

        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x4000 | (mov_y & 0x7F) | ((mov_x & 0x7F) << 7);
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "moverx") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        int mov_x = atoi(tokens[1]);

        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = mov_x & 0xFFF;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "movery") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        int mov_y = atoi(tokens[1]);

        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = (mov_y & 0xFFF) | 0x1000;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "wheelup") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BFB;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "wheeldown") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BFC;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "leftdown") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2501;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "leftup") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2401;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "middledown") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2504;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "middleup") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2404;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "rightdown") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2502;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "rightup") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2402;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "button4down") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2508;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "button4up") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2408;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "button5down") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2510;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "button5up") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2410;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "rightwheeldown") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BEC;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "rightwheelup") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BED;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "leftwheeldown") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BEE;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "leftwheelup") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BEF;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "swingr") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BF0;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "swingl") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BF1;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "dpi") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        if (strcmp(tokens[1], "0") == 0)
            *buf = 0x2BF0;
        else if (strcmp(tokens[1], "1") == 0)
            *buf = 0x2BF1;
        else if (strcmp(tokens[1], "2") == 0)
            *buf = 0x2BF2;
        else if (strcmp(tokens[1], "3") == 0)
            *buf = 0x2BF3;
        else if (strcmp(tokens[1], "4") == 0)
            *buf = 0x2BF4;
        else if (strcmp(tokens[1], "5") == 0)
            *buf = 0x2BF5;
        else if (strcmp(tokens[1], "up") == 0)
            *buf = 0x2BF6;
        else if (strcmp(tokens[1], "down") == 0)
            *buf = 0x2BF4;
        else if (strcmp(tokens[1], "+") == 0)
            *buf = 0x2BF7;
        else if (strcmp(tokens[1], "-") == 0)
            *buf = 0x2BF5;
        else
            *buf = 0x2BFE;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "mode") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        if (strcmp(tokens[1], "0") == 0)
            *buf = 0x2BF8;
        else if (strcmp(tokens[1], "1") == 0)
            *buf = 0x2BF9;
        else
            *buf = 0x2BFA;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "stop") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BFD;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "null") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BFE;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "macroending") == 0)
    {
        unsigned short *buf = (unsigned short *)dst_buf;
        *buf = 0x2BFF;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "delay") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        int time = atoi(tokens[1]);

        if (time != 0)
        {
            *buf = 0x2800 + time;
            dst_buf = buf + 1;
        }
    }
    else if (strcmp(tokens[0], "delays") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        int time = atoi(tokens[1]);

        if (time != 0)
        {
            *buf = 0x2C00 + time;
            dst_buf = buf + 1;
        }
    }
    else if (strcmp(tokens[0], "goto") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        int gto = atoi(tokens[1]);
        *buf = 0xE000 + offset + gto;
        dst_buf = buf + 1;
    }
    else if (strcmp(tokens[0], "set") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        if (strcmp(tokens[1], "macrorepeat") == 0)
        {
            *buf = 0x2BE9;
            dst_buf = buf + 1;
        }
    }
    else if (strcmp(tokens[0], "clr") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        if (strcmp(tokens[1], "macrorepeat") == 0)
        {
            *buf = 0x2BE8;
            dst_buf = buf + 1;
        }
    }
    else if (strcmp(tokens[0], "clr") == 0)
    {
        if (token_cnt < 2)
			return dst_buf;

        unsigned short *buf = (unsigned short *)dst_buf;

        if (strcmp(tokens[1], "macrorepeat") == 0)
        {
            *buf = 0x2BE8;
            dst_buf = buf + 1;
        }
    }
    else if (strcmp(tokens[0], "ram") == 0)
    {
        // TODO
    }
    else if (strcmp(tokens[0], "var") == 0)
    {
        // TODO
    }
    else if (strcmp(tokens[0], "if") == 0)
    {
        // TODO
    }
    else if (strcmp(tokens[0], "v0") == 0)
    {
        // TODO
    }
    else
    {
	printf("Not handled %s\n",cmd_line);
    }

	return dst_buf;
}

int parse_script(void *dst_buf, int *writed_len, int dst_buf_size, const char *src, int len)
{
	char line[256];

	const char *cur_pos = src;
	const char *line_start = src;
	const char *end_pos = src + len;

	if (cur_pos == end_pos)
		return -1;

	void *out_pos = dst_buf;
	void *out_end = (char *)dst_buf + dst_buf_size;

	while (end_pos - cur_pos > 0 )
	{
		if (*cur_pos == '\n' || *cur_pos == '\r' || cur_pos == end_pos - 1)
		{
			if (cur_pos == end_pos - 1)
				cur_pos++;
			if (cur_pos - line_start > 2)
			{
				memset(line, 0, 256);
				memcpy(line, line_start, cur_pos - line_start);
				line_start = cur_pos + 1;

				if ((char *)out_end - (char *)out_pos > 0)
                {
                    out_pos = parse_cmd(out_pos, (char *)out_end - (char *)out_pos, line, 0);
                    *writed_len = ((char *)out_pos - (char *)dst_buf);
                }
				else
					return -1;
			}
			else
			line_start = cur_pos + 1;
		}
		cur_pos++;
	}

	return 0;
}
