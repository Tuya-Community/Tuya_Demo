/*
 * tuya_utils.c
 *
 *  Created on: 2017-9-5
 *      Author: echo
 */
#include "tuya_utils.h"

u8 check_sum(u8 *pbuf,u16 len)
{
    u16 i = 0;
    u8	ck_sum = 0;

    for(i = 0;i<len ;i++)
        ck_sum += pbuf[i];
    return ck_sum;
}

u8 check_num(u8 *buf,u8 num)
{
    u8 i = 0;

    for(;i < buf[0];i++){
        if(buf[i+1] == num){
            return 1;
        }
    }
    return 0;
}

void hextoascii(u8 *hexbuf,u8 len,u8 *ascbuf)
{
    u8 i =0,j =0,temp = 0;

    for(i = 0;i<len;i++){
        temp = (hexbuf[i]>>4)&0xf;
        if((temp >=0)&&(temp <=9)){
            ascbuf[j] = temp + 0x30;
        }
        else{
            ascbuf[j] = temp + 87;
        }
        j++;
        temp = (hexbuf[i])&0xf;
        if((temp >=0)&&(temp <=9)){
            ascbuf[j] = temp + 0x30;
        }
        else{
            ascbuf[j] = temp + 87;
        }
        j++;
    }
    ascbuf[j] = 0x0;
}
void tuya_time_change_local(u8 *hexbuf,u8 len)
{

//     u8 i =0;
//    glxfatmeter.local_time=0;
//    for(i=0;i<(13-3);i++)
//    {
//        glxfatmeter.local_time=glxfatmeter.local_time*10+(hexbuf[i]&0x0F);
//    }
//    glxfatmeter.rec_time[10]=0x30;
//    glxfatmeter.rec_time[11]=0x30;
//    glxfatmeter.rec_time[12]=0x30;
   //TUYA_LOG("%d\r\n",glxfatmeter.local_time);
}
void tuya_time_change_update(u32 time)
{
    u8 i =0;
    u32 temp=time;
    for(i=0;i<10;i++)
    {
        //glxfatmeter.rec_time[10-1-i]=(temp%10)+0x30;
        temp/=10;      
    }
}


void asciitohex(u8 *ascbuf,u8 *hexbuf)
{
    u8 i =0,j =0;

    while(ascbuf[i]){
        j++;
        if((ascbuf[i] >= 0x30)&&(ascbuf[i] <= 0x39)){
            hexbuf[j] = ((ascbuf[i] - 0x30)<<4);
        }
        else if((ascbuf[i] >= 65)&&(ascbuf[i] <= 70)){
            hexbuf[j] = ((ascbuf[i] - 55)<<4);
        }
        else if((ascbuf[i] >= 97)&&(ascbuf[i] <= 102)){
            hexbuf[j] = ((ascbuf[i] - 87)<<4);
        }
        i++;
        if((ascbuf[i] >= 0x30)&&(ascbuf[i] <= 0x39)){
            hexbuf[j] |= (ascbuf[i] - 0x30);
        }
        else if((ascbuf[i] >= 65)&&(ascbuf[i] <= 70)){
            hexbuf[j] |= (ascbuf[i] - 55);
        }
        else if((ascbuf[i] >= 97)&&(ascbuf[i] <= 102)){
            hexbuf[j] |= (ascbuf[i] - 87);
        }
        i++;
    }
    hexbuf[0] = j;
}

u8 hex2int(u8 mhex)
{
    switch(mhex){
        case '0':return 0;
        case '1':return 1;
        case '2':return 2;
        case '3':return 3;
        case '4':return 4;
        case '5':return 5;
        case '6':return 6;
        case '7':return 7;
        case '8':return 8;
        case '9':return 9;
        case 'a':
        case 'A':return 10;
        case 'b':
        case 'B':return 11;
        case 'c':
        case 'C':return 12;
        case 'd':
        case 'D':return 13;
        case 'e':
        case 'E':return 14;
        case 'f':
        case 'F':return 15;
        default:return -1;
    }
}

char hexstr2int(u8 *hexstr,int len,u8 *sum)
{
    *sum = 0;
    int value;
    for(int i=0;i<len;i++){
        value = hex2int(hexstr[i]);
        if(value == -1)return 0;
        (*sum)=(*sum)<<4;
        (*sum)+=value;
    }
    return 1;
}

char hexstr2hex(u8 *hexstr,int len,u8 *hex)
{
	for(u8 i=0;i<len;i+=2){
		if(hexstr2int(&hexstr[i],2,&hex[i/2]) == 0)
			return 0;
	}
	return 1;
}

/*
// C prototype : void HexToStr(char *pszDest, byte *pbSrc, int nLen)
// parameter(s): [OUT] pszDest - 存放目标字符串
//	[IN] pbSrc - 输入16进制数的起始地址
//	[IN] nLen - 16进制数的字节数
// return value:
// remarks : 将16进制数转化为字符串
*/
void hex2hexstr(u8 *pbSrc, u16 nLen,char *pszDest)
{
	char	ddl, ddh;
	for (int i = 0; i < nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pszDest[i * 2] = ddh;
		pszDest[i * 2 + 1] = ddl;
	}

	pszDest[nLen * 2] = '\0';
}


//////////////////////tuya air process///////////////////////////////////////////////
const u8 crc8_table[256]={
    0x00,0x07,0x0e,0x09,0x1c,0x1b,0x12,0x15,0x38,0x3f,0x36,0x31,0x24,0x23,0x2a,0x2d,\
    0x70,0x77,0x7e,0x79,0x6c,0x6b,0x62,0x65,0x48,0x4f,0x46,0x41,0x54,0x53,0x5a,0x5d,\
    0xe0,0xe7,0xee,0xe9,0xfc,0xfb,0xf2,0xf5,0xd8,0xdf,0xd6,0xd1,0xc4,0xc3,0xca,0xcd,\
    0x90,0x97,0x9e,0x99,0x8c,0x8b,0x82,0x85,0xa8,0xaf,0xa6,0xa1,0xb4,0xb3,0xba,0xbd,\
    0xc7,0xc0,0xc9,0xce,0xdb,0xdc,0xd5,0xd2,0xff,0xf8,0xf1,0xf6,0xe3,0xe4,0xed,0xea,\
    0xb7,0xb0,0xb9,0xbe,0xab,0xac,0xa5,0xa2,0x8f,0x88,0x81,0x86,0x93,0x94,0x9d,0x9a,\
    0x27,0x20,0x29,0x2e,0x3b,0x3c,0x35,0x32,0x1f,0x18,0x11,0x16,0x03,0x04,0x0d,0x0a,\
    0x57,0x50,0x59,0x5e,0x4b,0x4c,0x45,0x42,0x6f,0x68,0x61,0x66,0x73,0x74,0x7d,0x7a,\
    0x89,0x8e,0x87,0x80,0x95,0x92,0x9b,0x9c,0xb1,0xb6,0xbf,0xb8,0xad,0xaa,0xa3,0xa4,\
    0xf9,0xfe,0xf7,0xf0,0xe5,0xe2,0xeb,0xec,0xc1,0xc6,0xcf,0xc8,0xdd,0xda,0xd3,0xd4,\
    0x69,0x6e,0x67,0x60,0x75,0x72,0x7b,0x7c,0x51,0x56,0x5f,0x58,0x4d,0x4a,0x43,0x44,\
    0x19,0x1e,0x17,0x10,0x05,0x02,0x0b,0x0c,0x21,0x26,0x2f,0x28,0x3d,0x3a,0x33,0x34,\
    0x4e,0x49,0x40,0x47,0x52,0x55,0x5c,0x5b,0x76,0x71,0x78,0x7f,0x6a,0x6d,0x64,0x63,\
    0x3e,0x39,0x30,0x37,0x22,0x25,0x2c,0x2b,0x06,0x01,0x08,0x0f,0x1a,0x1d,0x14,0x13,\
    0xae,0xa9,0xa0,0xa7,0xb2,0xb5,0xbc,0xbb,0x96,0x91,0x98,0x9f,0x8a,0x8d,0x84,0x83,\
    0xde,0xd9,0xd0,0xd7,0xc2,0xc5,0xcc,0xcb,0xe6,0xe1,0xe8,0xef,0xfa,0xfd,0xf4,0xf3
};
#define CRC8(CRC, C)  ((CRC = crc8_table[CRC ^ C]) & 0xFF)
/******************************************************************************
*
* Create an 8-bit crc table from the polynomial x^8 + x^2 + x + 1  ~ 0x07
*
******************************************************************************/
void made_session_key(u8 *input_buf,u8 len,u8 *output_buf)
{
    u8 i = 0,temp = 0;
    if(len < 16){
        for(i = 0;i < 16;i++){
            if(i >= len){
                temp = (unsigned char)((input_buf[i-len]+input_buf[i-len+1]));
                UNUSED(CRC8(output_buf[i],temp));
            }
            else{
            	UNUSED(CRC8(output_buf[i],input_buf[i]));
            }
        }
    }
    else{
        for(i = 0;i < 16;i++){
        	UNUSED(CRC8(output_buf[i],input_buf[i]));
        }
    }
}

u8 char_2_ascii(u8 data)
{
    u8 ret = 0xff;

    if((data >= 48)&&(data <= 57)){
        ret = data - 48;
    }
    else if((data >= 65)&&(data <= 70)){
        ret = data - 55;
    }
    else if((data >= 97)&&(data <= 102)){
        ret = data - 87;
    }
    return ret;
}

void str_to_hex(u8 *str_buf,u8 str_len,u8 *hex_buf)
{
    u8 data_tmp = 0,i = 0,j = 0;

    for(j = 0 ; j < str_len ;j++){
        data_tmp = char_2_ascii(str_buf[j]);
        if(data_tmp != 0xff){
            hex_buf[i] = (data_tmp << 4);
        }
        else{
            return;
        }
        j++;
        data_tmp = char_2_ascii(str_buf[j]);
        if(data_tmp != 0xff){
            hex_buf[i] += data_tmp;
        }
        else{
            return;
        }
        i++;
    }
}

u32 adv_report_parse(u8 type, data_t * p_advdata, data_t * p_typedata)
{
    u32  index = 0;
    u8 * p_data;
    p_data = p_advdata->p_data;
    while (index < p_advdata->data_len)
    {
        u8 field_length = p_data[index];
        u8 field_type   = p_data[index + 1];

        if (field_type == type)
        {
            p_typedata->p_data   = &p_data[index + 2];
            p_typedata->data_len = field_length - 1;
            return 0;//OPRT_OK
        }
        index += field_length + 1;
    }
    return 5;//OPRT_ERROR_NOT_FOUND
}

void ConverseArrayToBigEndian(u8 *srcArray,u32 *desBeData)
{
    *desBeData = (unsigned int)(srcArray[0]<<24) + (unsigned int)(srcArray[1]<<16) +
                    (unsigned int)(srcArray[2]<<8) + (unsigned int)srcArray[3];
}

void ConverseArrayToLittleEndian(u8 *srcArray,u32 *desLeData)
{
    *desLeData = (unsigned int)(srcArray[3]<<24) + (unsigned int)(srcArray[2]<<16) +
                    (unsigned int)(srcArray[1]<<8) + (unsigned int)srcArray[0];
}

u8 *getFirmwareVersion(void)
{
	static u8 ver[10]={0};
	if(ver[0] == 0x0)
	{
		memset(ver,0x0,10);
		//ver[0]=soft_ver_high+0x30;
		//ver[1]='.';
		//ver[2]=soft_ver_low+0x30;
	}
	return ver;
}
