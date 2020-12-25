#ifndef __TUYA_BLE_COMMON_H__
#define __TUYA_BLE_COMMON_H__
//tuya SDK
#include "tuya_ble_api.h"
#include "tuya_ble_mutli_tsf_protocol.h"
#include "tuya_ble_internal_config.h"
#include "tuya_ble_data_handler.h"
//#include "tuya_ble_cryption_api.h"
#include "tuya_ble_main.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_type.h"
#include "tuya_ble_unix_time.h"
//#include "tuya_ble_app_uart_common_handler.h"

#include "./tlsr/tuya_rf_test.h"
#include "./tlsr/tuya_log.h"
#include "./tlsr/tuya_ota.h"
#include "./tlsr/ty_broad.h"
#include "./tlsr/nv.h"

//#include "tuya_user.h"
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "vendor/common/blt_soft_timer.h"

//prvite define
typedef enum{
    NORMAL_STATUS = 0,
    OTA_STARTING,
    OTA_DATA,
    OTA_END,
    OTA_ERROR,
}o_ota_status;




//#define H_ID_LEN            		20
#define P_ID_LEN            		8
#define D_ID_LEN            		16
//#define MAC_LEN             		12
//#define AUTH_KEY_LEN        		32
#define PASS_KEY_LEN        		6
//#define SESSION_LEN         		16
//#define RANDOM_LEN          		6






#define TICK_BASE_MS           		1000
#define IRQ_TIME0_INTERVAL     		1000  //5ms

#define AIR_FRAME_MAX    			256

#define TY_HEART_MSG_TYPE       	0
#define TY_SEARCH_PID_TYPE      	1
#define TY_CK_MCU_TYPE          	2
#define TY_REPORT_BT_STATE  	    3
#define TY_RESET_TYPE             	4
#define TY_SEND_CMD_TYPE           	6
#define TY_SEND_STATUS_TYPE        	7
#define QUERY_STATUS            	8
#define BLE_UNBODING            	9
#define QUERY_BLE_STATUS            0x0A

#define TY_SCAN_RSSI        		0x0e

#define TY_SEND_CONNECT_REQUEST     0xA5

#define TY_SEND_STORAGE_TYPE        0xE0
#define TY_SEND_TIME_SYNC_TYPE      0xE1
#define TY_SEND_ADV_TIME_WHEN_SLEEP 0xE2
#define TY_SEND_QUERY_STORAGE_NUM   0xE3
#define TY_SEND_RTCTIMER_CTL        0xE4
#define TY_SEND_LP_ENABLE_CTL       0xE5
#define TY_ACTIVE_DISCONNECT		0xE7
#define BLE_OTA_STATUS            	0xF0

#define TY_QUERY_MCU_VERSION		0xE8

#define PRODUCT_ID_LEN 8

#define BLE_DEBUG_ENABLE            0x00


extern u32 lp_enable_flag;

extern u8 ota_status;

#endif
