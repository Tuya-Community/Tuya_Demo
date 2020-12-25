#include <stdarg.h>
#include "tuya_ble_common.h"

#define UTC_BASE_YEAR 1970
#define MONTH_PER_YEAR 12
#define DAY_PER_YEAR 365
#define SEC_PER_DAY 86400
#define SEC_PER_HOUR 3600
#define SEC_PER_MIN 60


static void printchar(char **str, int c) {
	if (str) {
		**str = c;
		++(*str);
	} else
		(void) putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad) {
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr)
			++len;
		if (len >= width)
			width = 0;
		else
			width -= len;
		if (pad & PAD_ZERO)
			padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for (; width > 0; --width) {
			printchar(out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		printchar(out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		printchar(out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad,
		int letbase) {
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	while (u) {
		t = u % b;
		if (t >= 10)
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if (width && (pad & PAD_ZERO)) {
			printchar(out, '-');
			++pc;
			--width;
		} else {
			*--s = '-';
		}
	}

	return pc + prints(out, s, width, pad);
}

static int print(char **out, const char *format, va_list args) {
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if (*format == 's') {
				register char *s = (char *) va_arg( args, int );
				pc += prints(out, s ? s : "(null)", width, pad);
				continue;
			}
			if (*format == 'd') {
				pc += printi(out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if (*format == 'x') {
				pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'X') {
				pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if (*format == 'u') {
				pc += printi(out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char) va_arg( args, int );
				scr[1] = '\0';
				pc += prints(out, scr, width, pad);
				continue;
			}
		} else {
			out: printchar(out, *format);
			++pc;
		}
	}
	if (out)
		**out = '\0';
	va_end( args );
	return pc;
}

int u_vsprintf(char *buf, const char *format, va_list args)
{
	return print(&buf, format, args);
}

int u_vsnprintf(char *buf, unsigned char size,const char *format, va_list args)
{
	int n=0;
	n=print(&buf, format, args);
	if(n>=size)
	{
		buf[size-1]=0;
		n=size-1;
	}
	return n;
}

int u_snprintf(char *buf, unsigned int size,const char *format, ...)
{
	int n=0;
	va_list args;
	va_start(args, format );
	n=print(&buf, format, args);
	if(n>=size)
	{
		buf[size-1]=0;
		n=size-1;
	}
	return n;
}

long tuya_atol(char *s)
{
	long r = 0;
	int neg = 0;
	switch(*s)
	{
		case '-':
			neg = 1;
			/* 这里没有break */
		case '+':
			s++;
			break;
	}
	while(*s >= '0' && *s <= '9')
	{
		int n = *s++ - '0';
		if(neg)
			n = -n;
		r = r * 10 + n;
	}
	return r;
}




void tuya_memcpy(unsigned char *p,unsigned char *m,unsigned int num)
{
	char *str1 = (char *)p;
	const char *str2 = (const char *)m;

	while(num)
	{
		*str1=*str2;
		str1++;
		str2++;
		num--;
	}
}

int utc_to_local_time(unsigned int utc_sec,int time_zone, tuya_ble_time_struct_data_t* my_time)
{
    unsigned int sec;
    char *DayIndex[] = {"Sun.", "Mon.", "Tues.", "Wed.", "Thur.", "Fri.", "Sat."};

    /* 这里根据UTC时间戳计算出来的时间是零时区的时间，所以如果要转化成北京时间就需要多加8小时 */
    tuya_ble_utc_sec_2_mytime(utc_sec + (time_zone * SEC_PER_HOUR)/100, my_time, false);

    //tuya_log_d("%d-%d-%d %d:%d:%d %s\n", my_time->nYear, my_time->nMonth, my_time->nDay,
    //        my_time->nHour, my_time->nMin, my_time->nSec, DayIndex[my_time->DayIndex]);

    return 0;
}

char *	strstr(const char *_s1, const char *_s2)
{
	char* cur = _s1;
	char* cur_str2 = _s2;
	while (*cur != '\0')
	{
		const char* move = cur;
		const char* cur_str2 = _s2;
		while (*move != '\0'
			&&*cur_str2 != '\0'
			&&*move == *cur_str2)
		{
			move++;
			cur_str2++;
		}
		if (*move == '\0')
		{
			break;
		}
		if (*cur_str2 == '\0')
		{
			return cur;
		}
		++cur;
	}
	return NULL;
}

long atoll(char *s)
{
	long r = 0;
	int neg = 0;
	switch(*s)
	{
		case '-':
			neg = 1;
			/* 这里没有break */
		case '+':
			s++;
			break;
	}
	while(*s >= '0' && *s <= '9')
	{
		int n = *s++ - '0';
		if(neg)
			n = -n;
		r = r * 10 + n;
	}
	return r;
}

