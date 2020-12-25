/*
 * tuya_utils.h
 *
 *  Created on: 2017-9-5
 *      Author: echo
 */

#ifndef TUYA_UTILS_H_
#define TUYA_UTILS_H_

#include "./tuya_ble_type.h"

#ifndef   UNUSED
#define   UNUSED(v)   ((void)(v))
#endif

#define CNT_OF(_x)			(sizeof(_x)/sizeof(_x[0]))

#define offset_of(type , member)((u32) &((type *)0)->member)

#define container_of(ptr , type , member)({            \
		const typeof(((type *)0)->member)*__mptr = (ptr);      \
		(type *)((u8 *)__mptr - offsetof(type , member));})

u8 check_sum(u8 *pbuf,u16 len);

u8 check_num(u8 *buf,u8 num);

void hextoascii(u8 *hexbuf,u8 len,u8 *ascbuf);

void asciitohex(u8 *ascbuf,u8 *hexbuf);

u8 hex2int(u8 mhex);
char hexstr2int(u8 *hexstr,int len,u8 *sum);
char hexstr2hex(u8 *hexstr,int len,u8 *hex);

void made_session_key(u8 *input_buf,u8 len,u8 *output_buf);

u8 char_2_ascii(u8 data);

void str_to_hex(u8 *str_buf,u8 str_len,u8 *hex_buf);

void ConverseArrayToBigEndian(u8 *srcArray,u32 *desBeData);

void ConverseArrayToLittleEndian(u8 *srcArray,u32 *desLeData);

void tuya_time_change_local(u8 *hexbuf,u8 len);
void tuya_time_change_update();


typedef struct
{
	u8     * p_data;    /**< Pointer to data. */
	u16      data_len;  /**< Length of data. */
} data_t;

u32 adv_report_parse(u8 type, data_t * p_advdata, data_t * p_typedata);

u8 *getFirmwareVersion(void);
#endif /* TUYA_UTILS_H_ */
