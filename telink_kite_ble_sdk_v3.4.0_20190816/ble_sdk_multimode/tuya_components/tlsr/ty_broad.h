/*
 * ty_broad.h
 *
 *  Created on: 2016-12-30
 *      Author: Tom
 */

#ifndef _TY_BROAD_H_
#define _TY_BROAD_H_
#include "tuya_ble_type.h"
//#include "../../proj/tl_common.h"
//#include "../../utils/mutli_tsf_protocol.h"
//#include "../../utils/nv.h"
//#include "../../tuya/include/tuya_ble_common.h"


//#define BLE_FLASHBUFFER_ENABLE
//#define BLE_LOWPOWER_ENABLE

#define TY_SPP_MODE 1
///////////////////////////////////////////////////////////////////////////////////////

typedef enum{
    TIMER_FIRST = 0,
    TIMER_OTA_TIMEOUT,
    TIMER_OTA_END,
    TIMER_BLEDATA_SEND,
    TIMER_MCU_HEARTBEAT,
    TIMER_REBOOT,
	TIMER_SCAN,
	TIMER_UPDATE_CONN_PARA,
	TIMER_RTC,
	TIMER_FLASH_DATA_REPORT,
	TIMER_UART_RX_TIMEOUT,
	TIMER_SCAN_RESULT,
	TIMER_ADV_CONNECT_REQUEST,
	TIMER_CONNECT_MONITOR=0x20,
	TIMER_IIC,
	TIMER_LIGHT,
    TIMER_ID_MAX,
}TIMER_ID_NUM;

typedef enum{
    DISABLE = 0,
    ENABLE,
}TY_FLAG;


typedef struct{
    u8 session_key[16];    //16
    u8 d_id[16];//16
    u8 d_id_flag;//1
    u8 pass_key[6];//6
    u8 pass_key_flag;//1
    u32 reset_cnt;//1
}b_ble_para;

typedef enum{
    USER_TEST_ID = 0,
    USER_PARA_ID,
    USER_FACTORY_TEST_ID,
}u_user_para_id;

extern u8 ty_ble_state;
extern u8 p_id[];
extern u8 uart_to_ble_enable;
extern u32 unix_time;
extern u32 rtc_timer_vaild;
extern u32 reset_cnt;
extern u8 time_sync_success;
extern u32 sysrun_seconds;
extern u32 sleep_adv_time;
extern u8 ble_report_ack_flag;
extern u8 time_req_type;
extern volatile u8 ty_factory_flag;

void ty_para_init(void);
void ty_para_read(u8 id, u16 len, u8 *buf);
void ty_para_write(u8 id, u16 len, u8 *buf);
void ty_ble_discon(void);
int ty_first_timer_proc(void);
void ty_ev_terminate_proc(void);
u32 tuya_timer_start(u8 timer_id,u32 time_ms_cnt);
u32 tuya_timer_delete(u8 timer_id);
void tuya_reboot_timer_start(u32 time_ms);
void tuya_para_init(void);

#endif /* _TY_BROAD_H_ */
