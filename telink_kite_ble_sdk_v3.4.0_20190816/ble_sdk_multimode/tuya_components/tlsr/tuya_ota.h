/*
 * tuya_ota.h
 *
 *  Created on: 2017-9-6
 *      Author: echo
 */

#ifndef BSP_OTA_H_
#define BSP_OTA_H_

#include "tuya_ble_type.h"
#include <stdint.h>


#define TUYA_OTA_VERSION 3

#define TUYA_OTA_TYPE    0


///#define	    APP_NEW_FW_START_ADR	            (0x41000)

//#define	    APP_NEW_FW_END_ADR	                (0x5C000)

#define	    APP_NEW_FW_MAX_SIZE                (124*1024)


#define     OTA_SETTINGS_ADR  0x6E000

typedef enum
{
	TUYA_OTA_STATUS_NONE,
	TUYA_OTA_STATUS_START,
	TUYA_OTA_STATUS_FILE_INFO,
	TUYA_OTA_STATUS_FILE_OFFSET,
    TUYA_OTA_STATUS_FILE_DATA,
    TUYA_OTA_STATUS_FILE_END,
	TUYA_OTA_STATUS_MAX,
}tuya_ota_status_t;

typedef struct
{
   uint16_t cmd;
   uint16_t data_length;
   uint8_t  *data;
}tuya_ota_cmd_t;

static enum e_ota_status_t{
	WAIT_FOR_START,
	WAIT_FOR_COPY,
	WAIT_FOR_EREASE,
	APP_DOWNLOADING,
	BOOT_DOWNLOADING,
	ENTER_BOOTLOADER=0xF0,
}e_ota_status;

typedef struct
{
	u32  firmware_addr;
	uint32_t firmware_file_version;
	uint32_t firmware_file_length;
	uint32_t firmware_file_crc;
	uint32_t firmware_image_crc_last;
	uint32_t firmware_image_offset_last;
	uint8_t  firmware_file_md5[16];
	u32  res[5];
}new_image_t;

typedef struct
{
	u8   ota_mac[8];
	u8   ota_adv_name[20];
	u8   ota_pid[8];
	u8   ota_did[16];
	u8   ota_autrkey[32];
}product_infor_t;

typedef struct
{
	  u32  settings_length;
	  u32  settings_crc32;
	  u32  settings_version;
	  u32  bootloader_version;
	  uint32_t firmware_file_version;
	  uint32_t firmware_file_length;
	  uint32_t firmware_file_crc;
	  uint8_t  firmware_file_md5[16];
	  uint32_t write_offset;
	  new_image_t new_image;
	  u32  ota_status;
	  u32  reset_count_max;
	  //product_infor_t product_infor;
	  u32  res[16];
}ota_settings_t;


void tuya_ota_proc(uint16_t cmd,uint8_t *recv_data,uint32_t recv_len);


uint32_t tuya_ota_init(void);

uint8_t tuya_ota_init_disconnect(void);

void tuya_ota_status_set(tuya_ota_status_t status);

tuya_ota_status_t tuya_ota_status_get(void);


void tuya_erase_new_firmware_bank(void);


#endif /* BSP_OTA_H_ */
