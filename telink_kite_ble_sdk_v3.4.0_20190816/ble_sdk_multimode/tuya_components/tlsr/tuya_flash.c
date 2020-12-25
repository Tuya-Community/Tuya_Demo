
/*
 *  tuya_flashbuffer.c
 *
 *  Created on: 2018-11-13
 *  Author: lsy
 */
//#include "../../port/include/tuya_port.h"
#include "tuya_ble_common.h"

#include <stdint.h>


#define	tuya_log_v(...)

static u8 init=0;

#define FLASH_BUFFER_START_ADDR   0x50000
#define FLASH_BUFFER_SECTOR_SIZE  5
#define FLASH_BUFFER_END_ADDR     (FLASH_BUFFER_START_ADDR+FLASH_BUFFER_SECTOR_SIZE*4*1024)


static u8 flash_buffer_status[FLASH_BUFFER_SECTOR_SIZE*16];

typedef struct _item
{
	unsigned int length;
	unsigned int crc32;
	unsigned int index;
	unsigned int flag;
	unsigned int res[4];
} item_header_t;

unsigned int write_page_addr=0;
unsigned int read_page_addr =0;

static uint32_t crc32_compute(uint8_t const * p_data, uint32_t size, uint32_t const * p_crc)
{
    uint32_t crc;
    crc = (p_crc == NULL) ? 0xFFFFFFFF : ~(*p_crc);
    for (uint32_t i = 0; i < size; i++)
    {
        crc = crc ^ p_data[i];
        for (uint32_t j = 8; j > 0; j--)
        {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }
    return ~crc;
}
u32 start_point=0;
u32 index_max=0;
u32 item_index=0;
static unsigned int tuya_flashbuffer_find_start_item()
{
	item_header_t item_header;
	u32 start_flag;
	u32 end_flag;
	u32 page_addr=FLASH_BUFFER_START_ADDR>>8,page_addr1=0,page_addr2=0;
    u32 data[200/4];

    //tuya_log_v("flashbuffer:find_start_item-0x%x-0x%x",FLASH_BUFFER_START_ADDR>>8,FLASH_BUFFER_END_ADDR>>8);
	while(page_addr<(FLASH_BUFFER_END_ADDR>>8))
	{
		tuya_bsp_flash_read_bytes((page_addr<<8), (u8*)&start_flag,4);//页头标识符
		tuya_bsp_flash_read_bytes((page_addr<<8)+4,  (u8*)&item_header,sizeof(item_header_t));//内容标记
		tuya_bsp_flash_read_bytes((page_addr<<8)+0xFF-4+1, (u8*)&end_flag,4);//页尾标识符
		//Printf("flashbuffer-w:-0x%x-0x%x-0x%x-0x%x\n",page_addr,start_flag,end_flag,item_header.flag);
		//tuya_log_v("flashbuffer-f:0x%x-0x%x-0x%x-0x%x",page_addr,start_flag,end_flag,item_header.flag);

		if(start_flag==0xAABBCCDD&&end_flag==0xDDCCBBAA)
		{//从写入的起始点向后查找第一个可读取的点
			if(item_header.flag==0xFFFFFFFF)
			{
				if(item_header.length<=200)
				{
					tuya_bsp_flash_read_bytes((page_addr<<8)+4+sizeof(item_header_t),(u8*)data,item_header.length);//内容
					if(crc32_compute((u8*)data,item_header.length,NULL)==item_header.crc32)
					{
						//tuya_log_v("flashbuffer:-0x%x-used page\n",page_addr);
						flash_buffer_status[page_addr-(FLASH_BUFFER_START_ADDR>>8)]=0x01;
						if(item_header.index>index_max)
						{
							index_max=item_header.index;
							page_addr1=page_addr;
						}
					}
					else
					{
						tuya_log_v("flashbuffer:-0x%x-damge page",page_addr);
					}
				}
				else
				{

				}
			}
		}
		page_addr++;
	}
	if(page_addr1==0)
	{
		page_addr1=FLASH_BUFFER_START_ADDR>>8;
	}
	page_addr=page_addr1;
	//tuya_log_v("flashbuffer:find_start_item-0x%x-0x%x",FLASH_BUFFER_START_ADDR>>8,FLASH_BUFFER_END_ADDR>>8);
	while(page_addr<(FLASH_BUFFER_END_ADDR>>8))
	{//从最大的起始点向后查找第一个可写入的点
		tuya_bsp_flash_read_bytes((page_addr<<8), (u8*)&start_flag,4);//页头标识符
		tuya_bsp_flash_read_bytes((page_addr<<8)+4,  (u8*)&item_header,sizeof(item_header_t));//内容标记
		tuya_bsp_flash_read_bytes((page_addr<<8)+0xFF-4+1, (u8*)&end_flag,4);//页尾标识符
		if(start_flag==0xFFFFFFFF&&end_flag==0xFFFFFFFF)
		{
			page_addr2=page_addr;
			break;
		}
		page_addr++;
	 }

	if(page_addr2==0)
	{//没有可以写入的点，强制从最大索引下一个扇区写入
		write_page_addr=page_addr1;
		write_page_addr=(write_page_addr<<8)>>12;
		write_page_addr+=1;
		write_page_addr=(write_page_addr<<12)>>8;
		if(write_page_addr>=FLASH_BUFFER_END_ADDR>>8)
		{
			write_page_addr=FLASH_BUFFER_START_ADDR>>8;
		}
		tuya_bsp_flash_erease_sector(write_page_addr<<8);
	}
	else
	{//否则从可写入点开始写入
		write_page_addr=page_addr2;
	}
	item_index=index_max+1;
	start_point=write_page_addr;
    tuya_log_v("flashbuffer:index_max=%d,write_page=0x%0x\n",index_max,write_page_addr);
	//while(1);
	return 0;
}

unsigned int tuya_flashbuffer_erease_one_item(unsigned int page_addr)
{
	item_header_t item_header;
	u32 start_flag=0;
	u32 end_flag  =0;

	if(!init)         return 1;

	if((page_addr==0)||(page_addr<(FLASH_BUFFER_START_ADDR>>8))||(page_addr>(FLASH_BUFFER_END_ADDR>>8)))
	{//无效页地址
		return 2;
	}

	flash_buffer_status[page_addr-(FLASH_BUFFER_START_ADDR>>8)]=0x00;

	tuya_bsp_flash_read_bytes((page_addr<<8), (u8*)&start_flag,4);//页头标识符
	tuya_bsp_flash_read_bytes((page_addr<<8)+0xFF-4+1,(u8*)&end_flag,4);//页尾标识符
	if((start_flag!=0xAABBCCDD)||(end_flag!=0xDDCCBBAA))
	{//没有有效数据，不用擦除
		return 0;
	}
	tuya_bsp_flash_read_bytes((page_addr<<8)+4, (u8*)&item_header,sizeof(item_header_t));//内容标记
	item_header.flag=0xFFFFFFFE;//标记为已读取
	tuya_bsp_flash_write_bytes((page_addr<<8)+4,(u8*)&item_header,sizeof(item_header_t));//内容标记
	tuya_log_v("flashbuffer:erease page-0x%0x",page_addr);
	return 0;
}

unsigned int tuya_flashbuffer_write_one_item(unsigned char*item_data,unsigned short item_size)
{
	item_header_t item_header;
	u32 start_flag=0xAABBCCDD;
	u32 end_flag  =0xDDCCBBAA;
	u32 temp=0;
	u8 write_success=4;

	if(!init)         return 1;
	if(item_size>200) return 2;

	memset(&item_header,0xFF,sizeof(item_header_t));
    //1.地址检验
	if(write_page_addr>=(FLASH_BUFFER_END_ADDR>>8))
	{
		write_page_addr=FLASH_BUFFER_START_ADDR>>8;
	}
	if(write_page_addr<=(FLASH_BUFFER_START_ADDR>>8))
	{
		write_page_addr=FLASH_BUFFER_START_ADDR>>8;
	}
	if((write_page_addr<<8)%(4*1024)==0)
	{//4*1024起始页 擦除
		tuya_bsp_flash_erease_sector(write_page_addr<<8);
		//Printf("flashbuffer:write-erease start page-0x%0x\n",write_page_addr);
	}

	//2.写入数据
	item_header.length=item_size;
	item_header.index=item_index;
	item_header.flag=0xFFFFFFFF;
	item_header.crc32=crc32_compute(item_data,item_size,NULL);
	tuya_bsp_flash_write_bytes((write_page_addr<<8),  (u8*)&start_flag,4);//页头标识符
	tuya_bsp_flash_write_bytes((write_page_addr<<8)+4, (u8*)&item_header,sizeof(item_header_t));//内容标记
	tuya_bsp_flash_write_bytes((write_page_addr<<8)+4+sizeof(item_header_t), item_data, item_size);//内容
	tuya_bsp_flash_write_bytes((write_page_addr<<8)+0xFF-4+1, (u8*)&end_flag,4);//页尾标识符


	//3.读取校验
	u8 data[200];
//	if(data==NULL)
//	{
//		return 3;
//	}
	temp=write_page_addr;
	start_flag=0;
	end_flag=0;
	memset(&item_header,0,sizeof(item_header));
	tuya_bsp_flash_read_bytes((temp<<8), (u8*)&start_flag, 4);//页头标识符
	tuya_bsp_flash_read_bytes((temp<<8)+4, (u8*)&item_header,sizeof(item_header_t));//内容标记
	tuya_bsp_flash_read_bytes((temp<<8)+0xFF-4+1 ,(u8*)&end_flag,4);//页尾标识符
	if(start_flag==0xAABBCCDD&&end_flag==0xDDCCBBAA)
	{
		if(item_header.flag==0xFFFFFFFF)
		{
			if(item_header.length<=200)
			{
				tuya_bsp_flash_read_bytes((temp<<8)+4+sizeof(item_header_t), data,item_header.length);//内容
				if(crc32_compute(data,item_header.length,NULL)==item_header.crc32)
				{
					tuya_log_d("flashbuffer:write item-0x%0x-%d-%d",write_page_addr,item_data[0],item_size);
					write_success=0;
					item_index++;
					read_page_addr=write_page_addr;
					flash_buffer_status[write_page_addr-(FLASH_BUFFER_START_ADDR>>8)]=0x01;
				}
			}
		}
	}

	//4.循环储存,擦除最头部的一条
	temp=write_page_addr+16;
	if(temp>=(FLASH_BUFFER_END_ADDR>>8))
	{
		temp=(FLASH_BUFFER_START_ADDR>>8)+(temp-(FLASH_BUFFER_END_ADDR>>8));
	}
	if(flash_buffer_status[temp-(FLASH_BUFFER_START_ADDR>>8)])
	{
		tuya_flashbuffer_erease_one_item(temp);
	}


	write_page_addr++;
	if(write_page_addr>=(FLASH_BUFFER_END_ADDR>>8))
	{
		write_page_addr=FLASH_BUFFER_START_ADDR>>8;
	}
	//tuya_free(data);

	return write_success;
}


u8 tuya_flashbuffer_get_item_total_num()
{
	u32 read_cnt=0,page_addr=0,num=0;

	while(read_cnt<FLASH_BUFFER_SECTOR_SIZE*16)
	{
		if(flash_buffer_status[read_cnt]==0x01)
		{
			num++;
		}
		read_cnt++;
	}
	return num;
}


unsigned int tuya_flashbuffer_read_one_item(unsigned char *data,unsigned int *len,u8 stytle)
{
	item_header_t item_header;
	u32 read_flag=0;
	u32 start_flag;
	u32 end_flag;
	u32 page_addr=0,read_cnt=0;
	*len=0;
	if(!init)         return 0;

	//1.地址检验
	if(read_page_addr>=(FLASH_BUFFER_END_ADDR>>8))
	{
		read_page_addr=FLASH_BUFFER_START_ADDR>>8;
	}
	if(read_page_addr<(FLASH_BUFFER_START_ADDR>>8))
	{
		read_page_addr=FLASH_BUFFER_START_ADDR>>8;
	}

	while(read_cnt<FLASH_BUFFER_SECTOR_SIZE*16)
	{

		if(flash_buffer_status[read_page_addr-(FLASH_BUFFER_START_ADDR>>8)]==0x01)
		{

			tuya_bsp_flash_read_bytes((read_page_addr<<8), (u8*)&start_flag,4);//页头标识符
			tuya_bsp_flash_read_bytes((read_page_addr<<8)+4, (u8*)&item_header, sizeof(item_header_t));//内容标记
			tuya_bsp_flash_read_bytes((read_page_addr<<8)+0xFF-4+1,(u8*)&end_flag, 4);//页尾标识符

//			if(read_page_addr==FLASH_BUFFER_START_ADDR>>8)
//			{
//				tuya_log_v("fl0-%x-%x",start_flag,end_flag);
//			}

			if(start_flag==0xAABBCCDD&&end_flag==0xDDCCBBAA)
			{
				if(item_header.flag==0xFFFFFFFF)
				{
					if(item_header.length<=200)
					{
						tuya_bsp_flash_read_bytes((read_page_addr<<8)+4+sizeof(item_header_t),data, item_header.length);//内容
						if(crc32_compute(data,item_header.length,NULL)==item_header.crc32)
						{
							*len=item_header.length;
							page_addr=read_page_addr;
							if(stytle==0)
							{//读取成功后 读取指针不变，必须等待擦除 变为无效后，才读取下一个数据源

							}
							else
							{//读取成功后 读取指针自动减一
								flash_buffer_status[read_page_addr-(FLASH_BUFFER_START_ADDR>>8)]=0x00;
								if(read_page_addr<=(FLASH_BUFFER_START_ADDR>>8))
								{
									read_page_addr=(FLASH_BUFFER_END_ADDR>>8)-1;
								}
								else
								{
									read_page_addr--;
								}
							}
							break;
						}
					}
				}
			}
			else
			{
				flash_buffer_status[read_page_addr-(FLASH_BUFFER_START_ADDR>>8)]=0x00;
			}
		}
		if(read_page_addr<=(FLASH_BUFFER_START_ADDR>>8))
		{
			read_page_addr=(FLASH_BUFFER_END_ADDR>>8)-1;
		}
		else
		{
			read_page_addr--;
		}
		read_cnt++;
	}
	if(page_addr!=0)
	{
		tuya_log_d("flashbuffer:read item=0x%x-0x%0x-0x%0x-0x%0x-%d",page_addr,read_page_addr,write_page_addr,data[0],item_header.index);
	}
	else
	{
		tuya_log_d("flashbuffer:no item");
	}
	return page_addr;
}

unsigned int tuya_flashbuffer_erease_all()
{
	if(init)
	{
		for(u16 i=0;i<FLASH_BUFFER_SECTOR_SIZE;i++)
		{
			tuya_bsp_flash_erease_sector(((FLASH_BUFFER_START_ADDR+i*4096)>>8)<<8);
		}
	}
	return 0;
}

unsigned int tuya_flashbuffer_init()
{
	if(!init)
	{
		tuya_flashbuffer_find_start_item();
		init=1;
	}
	return 0;
}


unsigned int tuya_flashbuffer_test()
{
//	unsigned int cnt=0,write=0,read=0;
//	unsigned int data_buffer[200/4];
//	unsigned int len=0,err=0;
//	unsigned int page_addr=0;
//
////	for(u16 i=0;i<FLASH_BUFFER_SECTOR_SIZE;i++)
////	{
////		tuya_bsp_flash_erease_sector(((FLASH_BUFFER_START_ADDR+i*4096)>>8)<<8);
////	}
//
//	tuya_flashbuffer_init();
//	for(u8 i=0;i<FLASH_BUFFER_SECTOR_SIZE*16;i++)
//	{
//		page_addr=tuya_flashbuffer_read_one_item((u8*)data_buffer,&len,0);
//		tuya_flashbuffer_erease_one_item(page_addr);
//		if(page_addr)
//			read++;
//		tuya_bsp_delay_ms(2);
//	}
//	tuya_log_v("start_point=%x,index_max=%d,write=%d,read=%d---------------------",start_point,index_max,write,read);
//	tuya_log_v("---------------------");
//	tuya_bsp_delay_ms(3000);
//	read=0;
//	for(u8 i=0;i<10;i++)
//	{
//		err=tuya_flashbuffer_write_one_item((u8*)&cnt,4);
//		cnt++;
//		if(!err)
//			write++;
//		tuya_bsp_delay_ms(2);
//	}
//
////	for(u8 i=0;i<FLASH_BUFFER_SECTOR_SIZE*16+1;i++)
////	{
////		page_addr=tuya_flashbuffer_read_one_item((u8*)data_buffer,&len,0);
////		tuya_flashbuffer_erease_one_item(page_addr);
////		if(page_addr)
////			read++;
////		tuya_bsp_delay_ms(2);
////	}
//	tuya_log_v("start_point=%x,index_max=%d,write=%d,read=%d---------------------",start_point,index_max,write,read);
//	tuya_log_v("write=%d---------------------",write);
//    for(u8 i=0;i<5;i++)
//	{
//		page_addr=tuya_flashbuffer_read_one_item((u8*)data_buffer,&len,0);
//		tuya_bsp_delay_ms(2);
//	}
//	while(1);
}


void tuya_flash_init()
{
    tuya_flashbuffer_init();
}


u32 tuya_key_value_save(const char *key,unsigned char *value,unsigned short value_len)
{
	//return ef_set_env_blob(key, value, value_len);
	if(memcmp("SYS_RUN_SECONDS",key,strlen("SYSRUN_SECONDS"))==0)
	{
		NV_USER2_ITEM_SAVE(NV_USER2_ITEM_SYS_RUN_SECONDS, value, value_len);
	}
	else if(memcmp("RESET_CNT",key,strlen("RESET_CNT"))==0)
	{
		NV_USER2_ITEM_SAVE(NV_USER2_ITEM_RESET_CNT, value, value_len);
	}
	else if(memcmp("UNIX_TIME",key,strlen("RESET_CNT"))==0)
	{
		NV_USER2_ITEM_SAVE(NV_USER2_ITEM_UNIX_TIME, value, value_len);
	}
	else if(memcmp("USER_PID",key,strlen("USER_PID"))==0)
	{
		NV_USER3_ITEM_SAVE(NV_USER3_ITEM_USER_PID, value, value_len);
	}
	else if(memcmp("RTC_FLAG",key,strlen("RTC_FLAG"))==0)
	{
		NV_USER3_ITEM_SAVE(NV_USER3_ITEM_RTC_FLAG, value, value_len);
	}
	else if(memcmp("OTA_FLAG",key,strlen("OTA_FLAG"))==0)
	{
		NV_USER3_ITEM_SAVE(NV_USER3_ITEM_OTA_FLAG, value, value_len);
	}
	else if(memcmp("SLEEP_ADV_TIME",key,strlen("SLEEP_ADV_TIME"))==0)
	{
		NV_USER3_ITEM_SAVE(NV_USER3_ITEM_SLEEP_ADV_TIME, value, value_len);
	}
	else if(memcmp("LP_ENABLE_FLAG",key,strlen("LP_ENABLE_FLAG"))==0)
	{
		NV_USER3_ITEM_SAVE(NV_USER3_ITEM_LP_ENABLE_FLAG, value, value_len);
	}
	else if(memcmp("USER_MCU_VERSION",key,strlen("USER_MCU_VERSION"))==0)
	{
		NV_USER3_ITEM_SAVE(NV_USER3_ITEM_USER_MCU_VERSION, value, value_len);
	}
	else
	{
		return 111;
	}
	return 0;
}

u32 tuya_key_value_load(const char *key,unsigned char *value,unsigned short value_len)
{
	if(memcmp("SYS_RUN_SECONDS",key,strlen("SYSRUN_SECONDS"))==0)
	{
		NV_USER2_ITEM_LOAD(NV_USER2_ITEM_SYS_RUN_SECONDS, value, value_len);
	}
	else if(memcmp("RESET_CNT",key,strlen("RESET_CNT"))==0)
	{
		NV_USER2_ITEM_LOAD(NV_USER2_ITEM_RESET_CNT, value, value_len);
	}
	else if(memcmp("UNIX_TIME",key,strlen("RESET_CNT"))==0)
	{
		NV_USER2_ITEM_LOAD(NV_USER2_ITEM_UNIX_TIME, value, value_len);
	}
	else if(memcmp("USER_PID",key,strlen("USER_PID"))==0)
	{
		NV_USER3_ITEM_LOAD(NV_USER3_ITEM_USER_PID, value, value_len);
	}
	else if(memcmp("RTC_FLAG",key,strlen("RTC_FLAG"))==0)
	{
		NV_USER3_ITEM_LOAD(NV_USER3_ITEM_RTC_FLAG, value, value_len);
	}
	else if(memcmp("OTA_FLAG",key,strlen("OTA_FLAG"))==0)
	{
		NV_USER3_ITEM_LOAD(NV_USER3_ITEM_OTA_FLAG, value, value_len);
	}
	else if(memcmp("SLEEP_ADV_TIME",key,strlen("SLEEP_ADV_TIME"))==0)
	{
		NV_USER3_ITEM_LOAD(NV_USER3_ITEM_SLEEP_ADV_TIME, value, value_len);
	}

	else if(memcmp("LP_ENABLE_FLAG",key,strlen("LP_ENABLE_FLAG"))==0)
	{
		NV_USER3_ITEM_LOAD(NV_USER3_ITEM_LP_ENABLE_FLAG, value, value_len);
	}
	else if(memcmp("USER_MCU_VERSION",key,strlen("USER_MCU_VERSION"))==0)
	{
		NV_USER3_ITEM_LOAD(NV_USER3_ITEM_USER_MCU_VERSION, value, value_len);
	}
	else
	{
		return 111;
	}
	return 0;
}
