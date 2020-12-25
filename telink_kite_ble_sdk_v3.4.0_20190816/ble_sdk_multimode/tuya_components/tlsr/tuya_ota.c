/*
 * tuya_ota.c
 *
 *  Created on: 2017-9-6
 *      Author: echo
 */
#include "tuya_ble_common.h"

#define TUYA_LOG(...)

#if   (__PROJECT_8258_MODULE__ || __PROJECT_8258_BLE_REMOTE__)
	#define	    NEW_FW_ADR	                    0x20000//(64<<12)
	#define     TELINK_OTA_BOOT_FLAG            (0x4B)
	#define     OTA_TYPE                        (2)
#else
	#error "ota unspourted"
#endif

extern u32	ota_program_offset;
u8 ota_status = NORMAL_STATUS;

static ota_settings_t ota_settings;

static tuya_ota_status_t tuya_ota_status;

uint32_t crc32_compute(uint8_t const * p_data, uint32_t size, uint32_t const * p_crc)
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

void tuya_erase_firmware_ready_for_ota(u32 start_addr_offset)
{
#if(OTA_TYPE==2)
	//for(u16 i = (start_addr_offset/4096); i <((APP_NEW_FW_MAX_SIZE-start_addr_offset)/4096); i++)
	//for(u16 i = 0; i <20; i++)
	//{
		bls_ota_clearNewFwDataArea(); //must
		//tuya_ble_nv_erase(ota_program_offset  + 4096*i,4096);
		//flash_erase_sector(0x20000+0 +(4096 * i));
	//}
#endif
}

void tuya_OTASetBootFlag(void)
{
	u32 flag = TELINK_OTA_BOOT_FLAG;
#if(OTA_TYPE==2)
	flash_write_page(ota_program_offset + 8, 1, (u8 *)&flag);		//Set FW flag
	flag =0;
	flash_read_page(ota_program_offset + 8, 1, (u8 *)&flag);		//Set FW flag
	if(flag==TELINK_OTA_BOOT_FLAG)
	{//擦除old_firmware,以方便下次更新
		flag =0;
		flash_write_page((ota_program_offset ? 0 : 0x20000) + 8, 1, (u8 *)&flag);	//Invalid flag
	}
#else
	tuya_ble_nv_erase(OTA_FLG_ADR,4096);
	tuya_ble_nv_write(OTA_FLG_ADR,(u8 *)&flag,4);
#endif

}


u32 tuya_OTASaveSettings(ota_settings_t* psettings)
{
	u32 err=0x12345678;
	psettings->settings_length=sizeof(ota_settings_t)-8;
	psettings->reset_count_max=18;

	psettings->settings_crc32=crc32_compute((u8 *)psettings+8,psettings->settings_length,NULL);
	tuya_bsp_flash_erease_sector(OTA_SETTINGS_ADR);
	tuya_ble_nv_write(OTA_SETTINGS_ADR, (u8 *)psettings,sizeof(ota_settings_t));
	tuya_log_d("Save settings_crc32=%x-%d\n",psettings->settings_crc32,sizeof(ota_settings_t));
	return err;
}


u32 tuya_OTALoadSettings(ota_settings_t* psettings)
{
	tuya_ble_nv_read(OTA_SETTINGS_ADR, (u8 *)psettings,sizeof(ota_settings_t));
//	tuya_log_d("/*******tuya_OTALoadSettings*********/\n");
//	//tuya_log_d("MD5=");
//	tuya_log_d("psettings_crc32=%x\nimage_length=%d\nimage_crc32=0x%x\nota_tatus=%d\n",
//			psettings->settings_crc32,psettings->new_image.firmware_file_length,
//			psettings->new_image.firmware_file_crc,psettings->ota_status);
	return 0;
}

u32 tuya_OTACheckSettings()
{
	u32 err=0;
	ota_settings_t ota_settings_temp;
	tuya_OTALoadSettings(&ota_settings_temp);
	if(ota_settings_temp.settings_crc32!=crc32_compute((u8 *)&ota_settings_temp+8,sizeof(ota_settings_t)-8,NULL))
	{
		tuya_log_d("tuya_OTACheckSettings crc err\n");
		memset((u8 *)&ota_settings_temp,0x00,sizeof(ota_settings_t));
		err+=tuya_OTASaveSettings(&ota_settings_temp);
	}
	else
	{
		//tuya_log_d("tuya_OTACheckSettings crc ok\n");
	}
	return err;
}



static u32 ota_timer_stop()
{
	tuya_timer_delete(TIMER_OTA_TIMEOUT);
	return 0;
}
s32 ota_timeout_handler()
{
	//tuya_reboot_timer_start(500);
	tuya_log_d("ota_timeout_handler\n");
	ota_status=0;
	tuya_ota_status=TUYA_OTA_STATUS_NONE;
	return -1;
}
static u32 ota_timer_start()
{
	tuya_timer_start(TIMER_OTA_TIMEOUT,10000);//10秒
	return 0;
}
u32 ota_timer_creat()
{
//	ota_timer_id=tuya_timer_create(TIMER_MODE_REPEATED,ota_timeout_handler);
	return 0;
}

#define TY_APP_VER_NUM 1

#define MAX_DFU_DATA_LEN  256

static uint32_t m_firmware_start_addr;          /**< Start address of the current firmware image. */
static uint32_t m_firmware_size_req;
static uint16_t current_package = 0;
static uint16_t last_package = 0;

static void tuya_ota_start_req(uint8_t*recv_data,uint32_t recv_len)
{
	ota_timer_start();
    uint8_t p_buf[70];
    uint8_t payload_len = 0;
    tuya_ble_ota_response_t response;

    uint32_t current_version = TY_APP_VER_NUM,ret=0;

//    if(tuya_ota_status!=TUYA_OTA_STATUS_NONE)
//    {
//        tuya_log_d("current ota status is not TUYA_OTA_STATUS_NONE  and is : %d !",tuya_ota_status);
//        return;
//    }

    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = TUYA_OTA_VERSION;
    p_buf[2] = 0;
    p_buf[3] = current_version>>24;
    p_buf[4] = current_version>>16;
    p_buf[5] = current_version>>8;
    p_buf[6] = current_version;
    p_buf[7] = MAX_DFU_DATA_LEN>>8;
    p_buf[8] = MAX_DFU_DATA_LEN;
    tuya_ota_status = TUYA_OTA_STATUS_START;
    payload_len = 9;

///    tuya_ble_commData_send(FRM_OTA_START_RESP,0,p_buf,payload_len,ENCRYPTION_MODE_SESSION_KEY);
    response.type=TUYA_BLE_OTA_REQ;
    response.p_data=p_buf;
    response.data_len=payload_len;
    ret= tuya_ble_ota_response(&response);

    //更改连接参数,大块写flash时，参数需要调大，否则某些手机容易断开
    bls_l2cap_requestConnParamUpdate (80, 80, 0, 400);

    tuya_log_d("tuya_ble_ota_response-%d",ret);
}



u32 tuya_ota_get_start_addr(void)
{
    u32 ota_start_addr=0;
#if(OTA_TYPE==2)
    ota_start_addr=ota_program_offset;
#else
    ota_start_addr=NEW_FW_ADR;
#endif
    return ota_start_addr;
}
static u32 ota_end_crc_cnt=0,ota_end_crc_last=0;
s32 ota_end_handler()
{
	tuya_ble_ota_response_t response;
    uint8_t buf[256],payload_len,state;
    u8 p_buf[2];
    static uint32_t crc_temp = 0;
    static uint32_t read_addr = 0;
    uint32_t cnt = ota_settings.new_image.firmware_image_offset_last/256;
    uint32_t remainder = ota_settings.new_image.firmware_image_offset_last % 256;

   // tuya_log_d("ota_end_handler  : 0x%04x",ota_end_crc_cnt);
    if(ota_end_crc_cnt==0)
    {
    	crc_temp=0;
    	read_addr=tuya_ota_get_start_addr();
    }
    for(uint32_t i = ota_end_crc_cnt; i<cnt; i++)
    {
        //memcpy(buf,(uint32_t *)read_addr,256);
    	tuya_ble_nv_read(read_addr,buf,256);

    	if(ota_end_crc_cnt==0) buf[8]=0x4B;//编译bin文件自带0x4b，写入是必须0xFF，覆盖

        crc_temp = crc32_compute(buf, 256, &crc_temp);
        read_addr += 256;
        ota_end_crc_cnt++;
        return 0;
    }

    if(remainder)
    {
        //memcpy(buf,(uint32_t *)read_addr,remainder);
    	tuya_ble_nv_read(read_addr,buf,remainder);
        crc_temp = crc32_compute(buf, remainder, &crc_temp);
        read_addr += remainder;
    }

    if(crc_temp!=ota_settings.new_image.firmware_file_crc)
    {
    	memset(&ota_settings, 0, sizeof(ota_settings_t));
    	state=1;
    	tuya_log_d("ota_end_handler crc fail");
    }
    else
    {
		tuya_OTASetBootFlag();//程序生效
		memset(&ota_settings.new_image, 0, sizeof(new_image_t));
		//ota_settings.write_offset   = 0;
    	state=0;
    	tuya_log_d("ota_end_handler crc ok");
    }

    ota_settings.ota_status=WAIT_FOR_EREASE;
    tuya_OTASaveSettings(&ota_settings);

	p_buf[0] = TUYA_OTA_TYPE;
	p_buf[1] = state;
	tuya_ota_status = TUYA_OTA_STATUS_NONE;
	payload_len = 2;
	response.type=TUYA_BLE_OTA_END;
	response.p_data=p_buf;
	response.data_len=payload_len;
	tuya_ble_ota_response(&response);
	tuya_ota_status = TUYA_OTA_STATUS_NONE;
	tuya_reboot_timer_start(2000);
    return -1;
}

void tuya_ota_end_timer_start()
{
	ota_end_crc_cnt=0;
	tuya_timer_start(TIMER_OTA_END,5);//10ms
}


static void tuya_ota_file_info_req(uint8_t*recv_data,uint32_t recv_len)
{
	ota_timer_start();
    uint8_t p_buf[30];
    uint8_t payload_len = 0;
    //uint32_t ack_sn = 0;
   // uint8_t encry_mode = 0;
    uint32_t file_version;
    uint32_t file_length;
    uint32_t file_crc;
    bool file_md5;
    // uint8_t file_md5[16];
    uint8_t state;
    tuya_ble_ota_response_t response;
    //recv_data=recv_data+13;

    if(tuya_ota_status!=TUYA_OTA_STATUS_START)
    {
        tuya_log_d("current ota status is not TUYA_OTA_STATUS_START  and is : %d !",tuya_ota_status);
        return;
    }

    if(recv_data[13-13]!=TUYA_OTA_TYPE)
    {
        tuya_log_d("current ota fm type is not 0!-%d-%d",recv_data[13-13],TUYA_OTA_TYPE);
        return;
    }

    file_version = recv_data[14+PRODUCT_ID_LEN-13]<<24;
    file_version += recv_data[15+PRODUCT_ID_LEN-13]<<16;
    file_version += recv_data[16+PRODUCT_ID_LEN-13]<<8;
    file_version += recv_data[17+PRODUCT_ID_LEN-13];

    if(memcmp(ota_settings.new_image.firmware_file_md5,&recv_data[17+PRODUCT_ID_LEN-13],16)==0)
    {
        file_md5 = true;
    }
    else
    {
    	memcpy(ota_settings.new_image.firmware_file_md5,&recv_data[17+PRODUCT_ID_LEN-13],16);
        file_md5 = false;
    }

    file_length = recv_data[42-13]<<24;
    file_length += recv_data[43-13]<<16;
    file_length += recv_data[44-13]<<8;
    file_length += recv_data[45-13];

    file_crc = recv_data[46-13]<<24;
    file_crc += recv_data[47-13]<<16;
    file_crc += recv_data[48-13]<<8;
    file_crc += recv_data[49-13];


    //if (memcmp(&recv_data[14], tuya_para.pid, PRODUCT_ID_LEN) == 0)
    if(1)
    {//支持夸pid,
        if((file_version >= 0x00)&&(file_length <= APP_NEW_FW_MAX_SIZE))
        {

            if(file_md5&&(ota_settings.new_image.firmware_file_version==file_version)&&(ota_settings.new_image.firmware_file_length==file_length)
                    &&(ota_settings.new_image.firmware_file_crc==file_crc))
            {
            	tuya_log_d("ota file same");
                state = 0;
            }
            else
            {
            	tuya_log_d("ota file length  : 0x%04x-0x%04x",ota_settings.new_image.firmware_file_length,file_length);
            	tuya_log_d("ota file  crc    : 0x%04x-0x%04x",ota_settings.new_image.firmware_file_crc,file_crc);
            	tuya_log_d("ota file version : 0x%04x-0x%04x",ota_settings.new_image.firmware_file_version,file_version);


            	tuya_log_d("ota file is not same");
                memset(&ota_settings.new_image, 0, sizeof(new_image_t));
                ota_settings.new_image.firmware_image_crc_last = 0;
                ota_settings.new_image.firmware_file_version = file_version;
                ota_settings.new_image.firmware_file_length = file_length;
                ota_settings.new_image.firmware_file_crc = file_crc;
                memcpy(ota_settings.new_image.firmware_file_md5,&recv_data[17+PRODUCT_ID_LEN-13],16);
                ota_settings.write_offset = ota_settings.new_image.firmware_image_offset_last;
                state = 0;
               // nrf_dfu_settings_write_and_backup(NULL);
                tuya_OTASaveSettings(&ota_settings);
            }

            m_firmware_start_addr = tuya_ota_get_start_addr();//APP_NEW_FW_START_ADR;
            m_firmware_size_req = ota_settings.new_image.firmware_file_length;

        }
        else
        {
            if(file_version <= TY_APP_VER_NUM)
            {
                tuya_log_d("ota file version error !");
                state = 2;
            }
            else
            {
                tuya_log_d("ota file length is bigger than rev space !");
                state = 3;
            }
        }

    }
    else
    {
        tuya_log_d("ota pid error !");
        state = 1;
    }

    memset(p_buf,0,sizeof(p_buf));
    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = state;
    if(state==0)
    {
        uint32_t crc_temp  = 0;
//        if(file_crc_check_in_flash(ota_settings.new_image.firmware_image_offset_last,&crc_temp)==0)
//        {
//            if(crc_temp != ota_settings.new_image.firmware_image_crc_last)
//            {
//            	ota_settings.new_image.firmware_image_offset_last = 0;
//            	ota_settings.new_image.firmware_image_crc_last = 0;
//            	ota_settings.write_offset = ota_settings.new_image.firmware_image_offset_last;
//                //nrf_dfu_settings_write_and_backup(NULL);
//            	tuya_OTASaveSettings(&ota_settings);
//            }
//        }

        p_buf[2] = ota_settings.new_image.firmware_image_offset_last>>24;
        p_buf[3] = ota_settings.new_image.firmware_image_offset_last>>16;
        p_buf[4] = ota_settings.new_image.firmware_image_offset_last>>8;
        p_buf[5] = (uint8_t)ota_settings.new_image.firmware_image_offset_last;
        p_buf[6] = ota_settings.new_image.firmware_image_crc_last>>24;
        p_buf[7] = ota_settings.new_image.firmware_image_crc_last>>16;
        p_buf[8] = ota_settings.new_image.firmware_image_crc_last>>8;
        p_buf[9] = (uint8_t)ota_settings.new_image.firmware_image_crc_last;
        tuya_ota_status = TUYA_OTA_STATUS_FILE_INFO;
        current_package = 0;
        last_package = 0;

        tuya_log_d("ota file length  : 0x%04x",ota_settings.new_image.firmware_file_length);
        tuya_log_d("ota file  crc    : 0x%04x",ota_settings.new_image.firmware_file_crc);
        tuya_log_d("ota file version : 0x%04x",ota_settings.new_image.firmware_file_version);
        //tuya_log_d("ota file md5 : 0x%04x",s_dfu_settings.progress.firmware_file_length);
        tuya_log_d("ota firmware_image_offset_last : 0x%04x",ota_settings.new_image.firmware_image_offset_last);
       // tuya_log_d("ota firmware_image_crc_last    : 0x%04x",ota_settings.new_image.firmware_image_crc_last);
       // tuya_log_d("ota firmware   write offset    : 0x%04x",ota_settings.write_offset);

    }
    payload_len = 26;


    response.type=TUYA_BLE_OTA_FILE_INFO;
    response.p_data=p_buf;
    response.data_len=payload_len;
    tuya_ble_ota_response(&response);
}



static void tuya_ota_offset_req(uint8_t*recv_data,uint32_t recv_len)
{
	ota_timer_start();
    uint8_t p_buf[5];
    uint8_t payload_len = 0;
    uint32_t ack_sn = 0;
  //  uint8_t encry_mode = 0;
    uint32_t offset,crc_temp;
    tuya_ble_ota_response_t response;

    if(tuya_ota_status!=TUYA_OTA_STATUS_FILE_INFO)
    {
        tuya_log_d("current ota status is not TUYA_OTA_STATUS_FILE_INFO  and is : %d !",tuya_ota_status);
        return;
    }


    offset  = recv_data[14-13]<<24;
    offset += recv_data[15-13]<<16;
    offset += recv_data[16-13]<<8;
    offset += recv_data[17-13];

    if((offset==0)&&(ota_settings.new_image.firmware_image_offset_last!=0))
    {
    	ota_settings.new_image.firmware_image_crc_last = 0;
    	ota_settings.new_image.firmware_image_offset_last = 0;
    	ota_settings.write_offset = ota_settings.new_image.firmware_image_offset_last;
        //nrf_dfu_settings_write_and_backup(NULL);
    }

//    offset=(offset>>12)<<12;//4k对齐
//    if(file_crc_check_in_flash(offset,&crc_temp)==0)
//    {
//    	ota_settings.new_image.firmware_image_offset_last=offset;
//    	ota_settings.new_image.firmware_image_crc_last=crc_temp;
//    	ota_settings.write_offset=ota_settings.new_image.firmware_image_offset_last;
//    }
    tuya_log_d("ota_offset is 0x%x-0x%x",ota_settings.new_image.firmware_image_offset_last,ota_settings.write_offset);

    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = ota_settings.new_image.firmware_image_offset_last>>24;
    p_buf[2] = ota_settings.new_image.firmware_image_offset_last>>16;
    p_buf[3] = ota_settings.new_image.firmware_image_offset_last>>8;
    p_buf[4] = (uint8_t)ota_settings.new_image.firmware_image_offset_last;


    ota_settings.ota_status=APP_DOWNLOADING;
    tuya_OTASaveSettings(&ota_settings);


    tuya_ota_status = TUYA_OTA_STATUS_FILE_OFFSET;

    payload_len = 5;

    //tuya_ble_commData_send(FRM_OTA_FILE_OFFSET_RESP,ack_sn,p_buf,payload_len,ENCRYPTION_MODE_SESSION_KEY);
    response.type=TUYA_BLE_OTA_FILE_OFFSET_REQ;
	response.p_data=p_buf;
	response.data_len=payload_len;
	tuya_ble_ota_response(&response);
}

//static u32 ota_data_len=0;
//static uint8_t  p_balloc_buf[256];
//s32 ota_data_timeout_handler()
//{
//
//}
//
//void tuya_ota_data_timer_start()
//{
//	ota_end_crc_cnt=0;
//	//tuya_timer_start(TIMER_OTA_DATA,5);//5ms
//	bsp_timer_start(ota_data_timeout_handler, 5*1000);
//}


static void tuya_ota_data_req(uint8_t*recv_data,uint32_t recv_len)
{
	ota_timer_start();
    uint8_t p_buf[2];
    uint8_t payload_len = 0;
    uint32_t ack_sn = 0,ret=1;
    uint8_t state = 0;
    uint16_t len;
    uint8_t  p_balloc_buf[256];
    tuya_ble_ota_response_t response;

    if((tuya_ota_status!=TUYA_OTA_STATUS_FILE_OFFSET)&&(tuya_ota_status!=TUYA_OTA_STATUS_FILE_DATA))
    {
        tuya_log_d("current ota status is not TUYA_OTA_STATUS_FILE_OFFSET  or TUYA_OTA_STATUS_FILE_DATA and is : %d !",tuya_ota_status);
        return;
    }

    state = 0;


    current_package = (recv_data[14-13]<<8)|recv_data[15-13];

    len = (recv_data[16-13]<<8)|recv_data[17-13];

    tuya_log_d("data package-%d-%d-%d-%d",current_package,len,ota_settings.write_offset,m_firmware_size_req);

    tuya_log_dumpHex("data00",20,recv_data,18);

    if((current_package!=(last_package+1))&&(current_package!=0))
    {
        tuya_log_d("ota received package number error.received package number : %d",current_package);
        state = 1;
    }
    else  if(len>MAX_DFU_DATA_LEN)
    {
        tuya_log_d("ota received package data length error : %d",len);
        state = 5;
    }
    else
    {
        uint32_t  write_addr = tuya_ota_get_start_addr() +  ota_settings.write_offset;//current_package*MAX_DFU_DATA_LEN;
        if(write_addr>=(tuya_ota_get_start_addr()+APP_NEW_FW_MAX_SIZE))
        {
            tuya_log_d("ota write addr error.");
            state = 1;
        }

        if(write_addr%4096==0)
        {
            if (tuya_ble_nv_erase(write_addr,4096) != 0)
            {
                tuya_log_d("ota Erase page operation failed");
                state = 4;
            }
            tuya_log_d("ota Erase page operation ");
        }

        if(state==0)
        {
            /* Allocate a buffer to receive data. */
            //p_balloc_buf = nrf_balloc_alloc(&m_buffer_pool);
           // if (p_balloc_buf == NULL)
           // {
            //    /* Operations are retried by the host; do not give up here. */
            //    tuya_log_d("cannot allocate memory buffer!");
            //    state = 4;
            //}
            //else
            {
                len = (recv_data[16-13]<<8)|recv_data[17-13];

                memcpy(p_balloc_buf, &recv_data[20-13], len);
                tuya_log_dumpHex("data0",20,p_balloc_buf,18);
                //ret_code_t ret = nrf_dfu_flash_store(write_addr, p_balloc_buf, len, on_flash_write);

                if(ota_settings.write_offset==0) p_balloc_buf[8]=0xFF;//编译bin文件自带0x4b，写入是必须0xFF，覆盖

                ret=tuya_ble_nv_write(write_addr, p_balloc_buf, len);
               // memset(p_balloc_buf,0,len);
               // ret=tuya_ble_nv_read(write_addr, p_balloc_buf, len);
                //tuya_log_dumpHex("data1",20,p_balloc_buf,18);
               // ret=0;
                //flash_write_page(write_addr,len,p_balloc_buf);//flash_write_page
                if (ret != 0)
                {
                    //on_flash_write((void*)p_balloc_buf);// free buffer
                    state = 4;
                }
                else
                {

                	if(ota_settings.write_offset==0) p_balloc_buf[8]=0x4B;//编译bin文件自带0x4b，写入是必须0xFF，覆盖

                	ota_settings.new_image.firmware_image_crc_last = crc32_compute(p_balloc_buf, len, &ota_settings.new_image.firmware_image_crc_last);
                	ota_settings.write_offset    += len;
                	ota_settings.new_image.firmware_image_offset_last += len;

                    if((ota_settings.new_image.firmware_image_offset_last%4096)==0)
                    {//每4K储一次进度
                        //nrf_dfu_settings_write_and_backup(NULL); //由于flash异步存储，此处操作有风险，会出现setting数据和实际存储的固件数据不一致，可在偏移量请求中增加实际校验规避。
                    	tuya_OTASaveSettings(&ota_settings);
                    }
                }
            }
        }
    }

    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = state;

    tuya_ota_status = TUYA_OTA_STATUS_FILE_DATA;

    payload_len = 2;

    tuya_log_d("ota data status-%d",state);
    //tuya_ble_commData_send(FRM_OTA_DATA_RESP,ack_sn,p_buf,payload_len,ENCRYPTION_MODE_SESSION_KEY);
    response.type=TUYA_BLE_OTA_DATA;
   	response.p_data=p_buf;
   	response.data_len=payload_len;
   	tuya_ble_ota_response(&response);

    if(state!=0)//出错，恢复初始状态
    {
        tuya_log_d("ota error so free!");
        tuya_ota_status = TUYA_OTA_STATUS_NONE;
        //tuya_ota_init_disconnect();
        memset(&ota_settings ,0, sizeof(ota_settings_t));
        //nrf_dfu_settings_write_and_backup(NULL);
        ota_settings.ota_status=WAIT_FOR_EREASE;
        tuya_OTASaveSettings(&ota_settings);
        tuya_reboot_timer_start(2000);
    }
    else
    {
        last_package = current_package;
    }

}





static void tuya_ota_end_req(uint8_t*recv_data,uint32_t recv_len)
{
	ota_timer_stop();
    uint8_t p_buf[2];
    uint8_t payload_len = 0;
    uint32_t ack_sn = 0;
   // uint8_t encry_mode = 0;
    tuya_ble_ota_response_t response;

    if(tuya_ota_status==TUYA_OTA_STATUS_NONE)
    {
        tuya_log_d("current ota status is TUYA_OTA_STATUS_NONE!");
        return;
    }


        uint8_t state;
        //static uint32_t sn;

        if (ota_settings.new_image.firmware_image_offset_last == m_firmware_size_req)
        {
            tuya_log_d("Whole firmware image received. Postvalidating.");

            uint32_t crc_temp  = 0;
//            if(file_crc_check_in_flash(ota_settings.new_image.firmware_image_offset_last,&crc_temp)==0)
//            {
//                if(ota_settings.new_image.firmware_image_crc_last != crc_temp)
//                {
//                    tuya_log_d("file crc check in flash diff from crc_last. crc_temp = 0x%04x,crc_last = 0x%04x",crc_temp,ota_settings.new_image.firmware_image_crc_last);
//                    ota_settings.new_image.firmware_image_crc_last = crc_temp;
//                }
//
//            }

            if(ota_settings.new_image.firmware_image_crc_last!=ota_settings.new_image.firmware_file_crc)
            {
                tuya_log_d("ota file crc check error,last_crc = 0x%04x ,file_crc = 0x%04x",ota_settings.new_image.firmware_image_crc_last,ota_settings.new_image.firmware_file_crc);
                state = 2;
            }
            else
            {
//            	tuya_OTASetBootFlag();//程序生效
//            	ota_settings.ota_status=WAIT_FOR_START;
//              memset(&ota_settings.new_image, 0, sizeof(new_image_t));
//              ota_settings.write_offset                  = 0;
              state = 0;
            }


        }
        else
        {

            state = 1;
        }


        if(state==0)
        {
        	tuya_log_d("ota download ok");
        	tuya_ota_end_timer_start();
             //ret = nrf_dfu_settings_write_and_backup((nrf_dfu_flash_callback_t)on_dfu_complete);
        }
        else
        {
        	tuya_log_d("ota download fail");
            tuya_log_d("ota crc error!");
            tuya_ota_status = TUYA_OTA_STATUS_NONE;
            //tuya_ota_init_disconnect();
            ota_settings.ota_status=WAIT_FOR_EREASE;
            memset(&ota_settings, 0, sizeof(ota_settings_t));
            tuya_OTASaveSettings(&ota_settings);
			p_buf[0] = TUYA_OTA_TYPE;
			p_buf[1] = state;
			tuya_ota_status = TUYA_OTA_STATUS_NONE;
			payload_len = 2;
			response.type=TUYA_BLE_OTA_END;
			response.p_data=p_buf;
			response.data_len=payload_len;
			tuya_ble_ota_response(&response);
			tuya_reboot_timer_start(2000);
        }
}


void tuya_ota_proc(uint16_t cmd,uint8_t *recv_data,uint32_t recv_len)
{
    switch(cmd)
    {
    case TUYA_BLE_OTA_REQ:
    	tuya_log_d("TUYA_BLE_OTA_REQ");
        tuya_ota_start_req(recv_data,recv_len);
        break;
    case TUYA_BLE_OTA_FILE_INFO:
    	tuya_log_d("TUYA_BLE_OTA_FILE_INFO");
        tuya_ota_file_info_req(recv_data,recv_len);
        break;
    case TUYA_BLE_OTA_FILE_OFFSET_REQ:
    	//tuya_log_d("TUYA_BLE_OTA_FILE_OFFSET_REQ");
        tuya_ota_offset_req(recv_data,recv_len);
        break;
    case TUYA_BLE_OTA_DATA:
        tuya_ota_data_req(recv_data,recv_len);
        break;
    case TUYA_BLE_OTA_END:
        tuya_ota_end_req(recv_data,recv_len);
        break;
    default:
    	tuya_log_d("unknown cmd");
        break;
    }

}

uint32_t tuya_ota_init()
{
	u32 offset=0;
	tuya_OTACheckSettings();
	tuya_OTALoadSettings(&ota_settings);
	//ota_settings.new_image.firmware_file_length=0;
	//ota_settings.ota_status=WAIT_FOR_EREASE;
	if(ota_settings.ota_status==WAIT_FOR_EREASE)
	{
		//tuya_erase_firmware_ready_for_ota(0);
		///bls_ota_clearNewFwDataArea();
		ota_settings.ota_status=WAIT_FOR_START;
		ota_settings.write_offset=0;
		ota_settings.new_image.firmware_file_length=0;
		tuya_OTASaveSettings(&ota_settings);
	}
	else if(ota_settings.ota_status==APP_DOWNLOADING)
	{
		if((ota_settings.new_image.firmware_image_offset_last%4096)!=0)
		{
			ota_settings.new_image.firmware_image_offset_last=0;
			ota_settings.write_offset=0;
		}
		else if(ota_settings.new_image.firmware_image_offset_last>APP_NEW_FW_MAX_SIZE)
		{
			ota_settings.new_image.firmware_image_offset_last=0;
			ota_settings.write_offset=0;
		}
		//ota_settings.new_image.firmware_image_offset_last=0;
		//ota_settings.write_offset=0;
		//tuya_erase_firmware_ready_for_ota(ota_settings.new_image.firmware_image_offset_last);
		//bls_ota_clearNewFwDataArea();
		ota_settings.ota_status=WAIT_FOR_START;
		tuya_OTASaveSettings(&ota_settings);
	}
	ota_timer_creat();
	ota_status=0;
	tuya_ota_status=TUYA_OTA_STATUS_NONE;
	return 0;
}

u32 tuya_get_ota_status()
{
	return tuya_ota_status;
}
