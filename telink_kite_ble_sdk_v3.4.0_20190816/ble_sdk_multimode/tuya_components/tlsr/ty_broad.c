#include "tuya_ble_common.h"
#include "tuya_ble_log.h"

#define DP_TEMP 1
#define DP_HUMI 2
//#define TY_DEVICE_PID "3224"

u32 reset_cnt=0;
u8 pair_ok = 0;
u32 unix_time=1569833410;
volatile u8 ty_factory_flag = 0;
u8 factory_test_reset = 0;
u8 reset_check_flag   = 0;
b_ble_para ble_para;
u8 ty_ble_state = UNKNOW_STATUS;
u32 rtc_timer_vaild=1;
u32  sleep_adv_time=ADV_INTERVAL_1S;
u32 sysrun_seconds=0;
u32 sysrun_ms=0;
u32 lp_enable_flag=0;
u8 ble_report_ack_flag=0;
u8 tbl_mac[8];
u8 time_req_type=1;
u32 ota_flag=1;
extern u8 tbl_scanRsp[28];// = {0x1B,0xFF,0x02,0x59};


u8 ty_ble_send_flag = 0;
u8 heatbeat_flag = 1;
extern u8 ota_status;
u8 uart_to_ble_enable=0;
void tuya_report_bt_state()
{
	//tuya_uart_send_ble_state();
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void ty_ev_terminate_proc(void)
{
    //ty_timer_stop_all()
    if(ENABLE == ble_para.pass_key_flag)
    {
        ty_ble_state = BONDING_UNCONN;                   
    }
    else
    {
        ty_ble_state = UNBONDING_UNCONN;
    }
    //if(gpio_read(GPIO_WAKEUP_MODULE))
    {
    	tuya_report_bt_state();
    }
    if(ota_status != NORMAL_STATUS) ota_status = NORMAL_STATUS;
}

extern s32 tuya_rtc_timer_update_handler(void);


int ty_first_timer_proc(void)
{

    if(1 == ty_factory_flag)
    {
    	//ty_uart_protocol_send(TY_REPORT_WORK_STATE_TYPE,&ty_ble_state,1);//濞戞挸锕﹂弫鎼佸箮閵夈儲鍟炲☉鎿勬嫹婵炲枴銈囩闁规亽鍎虫慨鎼佸箑閿燂拷
    	//tuya_report_bt_state();
        ty_factory_flag = 0;
        if(reset_check_flag == 1)
        {
            //TUYA_LOG("res:%d\r\n",ble_para.reset_cnt);
            //NV_USER1_ITEM_SAVE(NV_USER_ITEM_RESET_CNT, &ble_para.reset_cnt, 1);
            // tuya_flash_write_reset_cnt(&ble_para.reset_cnt, 1);
            tuya_key_value_save("RESET_CNT",&ble_para.reset_cnt,4);
        }
        return 10000;//start 10s as reset the count
    }
    else
    {
        if(reset_check_flag == 1)
        {
            //TUYA_LOG("cle:%d\r\n",ble_para.reset_cnt);
            ble_para.reset_cnt = 0;
            //NV_USER1_ITEM_SAVE(NV_USER_ITEM_RESET_CNT, &ble_para.reset_cnt, 1);
            //tuya_flash_read_reset_cnt(&ble_para.reset_cnt, 1);
            tuya_key_value_save("RESET_CNT",&ble_para.reset_cnt,4);
        }
        return -1;
    }
}

void ty_para_read(u8 id, u16 len, u8 *buf)
{
    NV_USER2_ITEM_LOAD(id, buf, len);
}

void ty_para_write(u8 id, u16 len, u8 *buf)
{
    NV_USER2_ITEM_SAVE(id, buf, len);
}

void ty_ble_discon(void)
{
    irq_disable();
    //cpu_reboot();

    sleep_us(5000*1000);//must be use WDT
    //start_reboot();
    //cpu_reset();
}


int bsp_timer_start(blt_timer_callback_t func, u32 interval_us)
{
	blt_soft_timer_delete(func);
	return blt_soft_timer_add(func,interval_us);
}

int bsp_timer_delete(blt_timer_callback_t func)
{
	return blt_soft_timer_delete(func);
}


static frm_trsmitr_proc_s ble_send_trsmitr;
static u8 ble_send_buf[TUYA_BLE_AIR_FRAME_MAX];
static u32 ble_send_len;

s32 ble_send_mux_packet()
{
    mtp_ret ret;
    u8 send_len = 0;
    u8 p_buf[20];
	ble_sts_t sta=0;
	static ble_sts_t last_sta=0;
	static mtp_ret   last_ret=0;
	static u8        retry_count=0;
	static u8 status=0;

//	if(type==FRM_OTA_END_RESP)
//	{
//		sta=ty_ble_notify(air_recv_packet.de_encrypt_buf[0],&air_recv_packet.de_encrypt_buf[1]);
//		return 0;
//	}
	ty_ble_send_flag = 1;
	//tuya_log_d("ble_send_mux_packet=%d",ble_send_trsmitr.subpkg_num);
	if(status==0)
	{
		trsmitr_init(&ble_send_trsmitr);
		status=1;
	}
	if(ble_send_trsmitr.subpkg_num==0)
	{
		last_sta=0;
		last_ret=0;
		retry_count=0;
		//TUYA_LOG("101%d",air_recv_packet.de_encrypt_buf[0]);
	}
	if(last_sta!=HCI_ERR_CONTROLLER_BUSY)
	{//闁告瑦鍨块敓鎴掔劍濠�澘鈻庨埄鍐╂闁圭櫢鎷�
//	    ret = trsmitr_send_pkg_encode(&ty_trsmitr_proc,type, &air_recv_packet.de_encrypt_buf[1], air_recv_packet.de_encrypt_buf[0]);
//		if(MTP_OK != ret && MTP_TRSMITR_CONTINUE != ret)
//		{
//			ty_ble_send_flag = 0;
//			status=0;
//			TUYA_LOG("11");
//	        return 0;
//	    }
		tuya_log_d("mux_packet:%d-%d",ble_send_trsmitr.subpkg_num,ret);
		ret = trsmitr_send_pkg_encode(&ble_send_trsmitr,TUYA_BLE_PROTOCOL_VERSION_HIGN,ble_send_buf, ble_send_len);
		if (MTP_OK != ret && MTP_TRSMITR_CONTINUE != ret)
		{
			ty_ble_send_flag=0;
			status=0;
			tuya_log_d("ble_send_mux_packet err=%d-%x-%d",ble_send_len,ble_send_trsmitr.subpkg_num,ret);
			return 0;//stop
		}
		//send_len = get_trsmitr_subpkg_len(&ty_trsmitr_proc_send);
		//memcpy(p_buf,get_trsmitr_subpkg(&ty_trsmitr_proc_send),send_len);
		//sta=tuya_ble_gatt_send_data(p_buf,send_len);
	}
	else
	{//缂備綀鍛暰闂佹彃绉磋ぐ鍌涚▔婵犲喚鍋ч柡浣哄瀹擄拷
		ret=last_ret;
	}
    //send_len = get_trsmitr_subpkg_len(&ty_trsmitr_proc);
    //memcpy(p_buf,get_trsmitr_subpkg(&ty_trsmitr_proc),send_len);
    //sta=ty_ble_notify(send_len,p_buf);

	send_len = get_trsmitr_subpkg_len(&ble_send_trsmitr);
	memcpy(p_buf,get_trsmitr_subpkg(&ble_send_trsmitr),send_len);
	sta=tuya_ble_gatt_send_data(p_buf,send_len);

	last_sta=sta;
	last_ret=ret;
	if(sta==BLE_SUCCESS)
	{
		retry_count=0;
	}
	//else if((sta==HCI_ERR_CONTROLLER_BUSY)||(sta==LL_ERR_TX_FIFO_NOT_ENOUGH))
	else if(sta!=TUYA_BLE_ERR_INVALID_STATE)
	{
		last_sta=HCI_ERR_CONTROLLER_BUSY;
		//TUYA_LOG("12");
	    retry_count++;
		if(retry_count<50)
		{
			return 20*1000;//20ms闂佹彃绉烽惁锟�0婵炲棴鎷�
		}
		else
		{
			//TUYA_LOG("13");
			tuya_log_d("ble send fail:busy");
			ty_ble_send_flag = 0;
			status=0;
			return 0;//stop
		}
	}
	else
	{
		tuya_log_d("ble send fail:%x",sta);
		ty_ble_send_flag = 0;
		status=0;
		return 0;//stop
	}

    if(MTP_OK == ret)
    {
    	//TUYA_LOG("14%d",air_recv_packet.de_encrypt_buf[0]);
    	//tuya_log_d("ble send ok-%d",ble_send_len);
    	tuya_log_d("ble send ok");
	    ty_ble_send_flag = 0;
	    status=0;
		return 0;//stop
	}
    else if(MTP_TRSMITR_CONTINUE)
	{
		return 6*1000;//loop
	}
	else
	{
		ty_ble_send_flag = 0;
		status=0;
		tuya_log_d("ble send fail=%d-%d",ble_send_len,ret);
		return 0;//stop
	}
}

s32 blt_soft_timer_bleData_send_handler()
{
	s32 temp=0;
	temp = ble_send_mux_packet();
	if(0 == temp)
	{
		return -1;
	}
	else
	{
		return temp;
	}
}
s32 chip_reboot(void)
{
    irq_disable();
    //start_reboot();
    sleep_us(8000*1000);//must be use WDT
    //start_reboot();
    start_reboot();
    while(1);
    return -1;
}
s32 first_timerout_handler(void)
{
    static u32 cnt=0;
    ++cnt;
    //tuya_log_v("ty_factory_flag:%d,cnt:%d\n",ty_factory_flag,cnt);
    if(cnt==1)
    {
    	++reset_cnt;
    	//tuya_key_value_save("RESET_CNT",&reset_cnt,4);
    	tuya_report_bt_state();
    	return 0;
    }
    else if(cnt>2)
    {
    	ty_factory_flag=0;
    	//tuya_log_v("ty_factory_flag:%d,cnt:%d\n",ty_factory_flag,cnt);
    	return -1;
    }
    
}

s32 conn_para_update_timerout_handler(void)
{
	//閺囧瓨鏁兼潻鐐村复閸欏倹鏆�825x娑撳﹨顕氭潻鐐村复闂傛挳娈ф稉宥堫洣闂呭繑鍓伴弴瀛樻暭閿涘本鏁肩亸蹇撴倵閿涘本鐓囨禍娑樻嫲flash閻╃鍙ч惃鍕惙娴ｆ粣绱濋崢瀣濞村鐦稉瀣剁礉閽冩繄澧�瑙勬閺傤厼绱� by lsy
	bls_l2cap_requestConnParamUpdate (50, 60, 0, 400);
	return -1;
}

u8 rtc_save_flag;//1:need save
//
s32 tuya_rtc_timer_update_handler(void)
{
	//if(NORMAL_STATUS != ota_status) return 0;
	static u32 tick_last=0;
	u32 tick_now=clock_time();
	u32 tick_pass=0,hour,min,sec;

	if(tick_now>tick_last)  tick_pass=tick_now-tick_last;
	else					tick_pass=0xFFFFFFFF-tick_last+tick_now;


	u32 seconds=tick_pass/CLOCK_SYS_CLOCK_1S;

    if(seconds==0) return (15*1000*1000);

    tick_last=tick_last+seconds*CLOCK_SYS_CLOCK_1S;

	sysrun_seconds+=seconds;
	unix_time+=seconds;

	if((sysrun_seconds%(30*60))==0)
	{//濮ｏ拷30 min娣囨繂鐡ㄦ稉锟藉▎鈩冩闂傦拷
		//tuya_key_value_save("SYS_RUN_SECONDS",&sysrun_seconds,4);
		//tuya_key_value_save("UNIX_TIME",&unix_time,4);
		rtc_save_flag=1;
	}
	if(rtc_save_flag)
	{
		if(blt_state!=BLS_LINK_STATE_CONN)
		{
			//閺堝娲块弬甯礉娣囨繂鐡ㄦ稉锟藉▎鈩冩闂傦拷
			//tuya_key_value_save("SYS_RUN_SECONDS",&sysrun_seconds,4);
			tuya_key_value_save("UNIX_TIME",&unix_time,4);
			rtc_save_flag = 0;
		}
	}

	hour=sysrun_seconds/3600;
	min=(sysrun_seconds-hour*3600)/60;
	sec=sysrun_seconds%60;
	//tuya_log_d("[rtc] %dh:%dmin:%ds-%d",hour,min,sec,unix_time);
	return (15*1000*1000);
}

void sysrun_ms_update_handler(void)
{
	//if(NORMAL_STATUS != ota_status) return 0;
	static u32 tick_last=0;
	u32 tick_now=clock_time();
	u32 tick_pass=0;

	if(tick_now>tick_last)  tick_pass=tick_now-tick_last;
	else					tick_pass=0xFFFFFFFF-tick_last+tick_now;

	u32 ms=tick_pass/CLOCK_SYS_CLOCK_1MS;

    if(ms==0) return;
	tick_last=tick_now;
    sysrun_ms+=ms;
}
static s32 tuya_conncet_monitor_callback()
{
    tuya_ble_connect_status_t connect_state = tuya_ble_connect_status_get();

    if( (connect_state == UNBONDING_UNAUTH_CONN)||(connect_state == BONDING_UNAUTH_CONN) )
    {
       // TUYA_BLE_LOG_DEBUG("ble disconncet because monitor timer timeout.");
        tuya_ble_gap_disconnect();
    }
    return -1;
}

void tuya_SHT30_callback()
{
	//your loop code here
	    uint8_t Data_Buffer[50] ={0};
	    uint32_t Data_Len = 14;
		int temperature = 200;
		int humidity = 99;
		uint8_t* Data_Buffer_ptr;
		Data_Buffer_ptr = Data_Buffer;
		/*
		 * dp data  buffer:  (Dp_id,Dp_type,Dp_len,Dp_data),(Dp_id,Dp_type,Dp_len,Dp_data),....
		 */
	    Data_Buffer[0] = DP_TEMP;
	    Data_Buffer[1] = DT_VALUE;
	    Data_Buffer[2] = DT_VALUE_LEN;
	    Data_Buffer[3] = temperature<<24;
	    Data_Buffer[4] = temperature<<16;
	    Data_Buffer[5] = temperature<<8;
	    Data_Buffer[6] = temperature<<0;
	    Data_Buffer[7] = DP_HUMI;
	    Data_Buffer[8] = DT_VALUE;
	    Data_Buffer[9] = DT_VALUE_LEN;
	    Data_Buffer[10] = humidity<<24;
	    Data_Buffer[11] = humidity<<16;
	    Data_Buffer[12] = humidity<<8;
	    Data_Buffer[13] = humidity<<0;

		tuya_ble_dp_data_report(Data_Buffer_ptr,Data_Len);//report temperature and humidity
		tuya_log_d("temperature=%d C\t humidity=%d% RH\t",temperature/10,humidity);
}

u32 tuya_timer_start(u8 timer_id,u32 time_ms_cnt)
{//濞夈劍鍓扮�姘閺冨爼妫块敍宀冪Т鏉╋拷3缁夋帪绱濇担搴″閼版濮搁幀浣风瑓閺冪姵纭堕幐澶嬫閹笛嗩攽閿涘矂娼担搴″閼版濮搁幀浣风瑝閸欐濂栭崫锟�
	u32 err_code=0;
	if(timer_id >= TIMER_ID_MAX)
	{
		return -1;
	}

	if(timer_id==TIMER_BLEDATA_SEND)
	{
		err_code=bsp_timer_start(blt_soft_timer_bleData_send_handler, time_ms_cnt*1000);
		return (!err_code);
	}
	else if(timer_id==TIMER_MCU_HEARTBEAT)
	{
		//extern s32 mcu_heartbeat_callback();
		//bsp_timer_start(mcu_heartbeat_callback, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_REBOOT)
	{
		bsp_timer_start(chip_reboot, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_RTC)
	{
		extern s32 tuya_rtc_timer_update_handler();
		if(blt_soft_timer_find_timer(tuya_rtc_timer_update_handler)==0)
		{
			bsp_timer_start(tuya_rtc_timer_update_handler, time_ms_cnt*1000);
		}
		return 0;
	}
	else if(timer_id==TIMER_FIRST)
	{
		bsp_timer_start(first_timerout_handler, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_FLASH_DATA_REPORT)
	{
		//extern s32 tuya_data_update_handler(void);
		//if(blt_soft_timer_find_timer(tuya_data_update_handler)==0)
		//{
		//	bsp_timer_start(tuya_data_update_handler, time_ms_cnt*1000);
		//}
		return 0;
	}
	else if(timer_id==TIMER_OTA_TIMEOUT)
	{
		extern s32 ota_timeout_handler();
		bsp_timer_start(ota_timeout_handler, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_OTA_END)
	{
			extern s32 ota_end_handler();
			bsp_timer_start(ota_end_handler, time_ms_cnt*1000);
			return 0;
	}
	else if(timer_id==TIMER_UART_RX_TIMEOUT)
	{
		extern s32 uart_timeout_handler(void);
		bsp_timer_start(uart_timeout_handler, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_SCAN)
	{
		extern int  scan_timeout_handler();
		bsp_timer_start(scan_timeout_handler, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_SCAN_RESULT)
	{
		extern int scan_result_handler();
		bsp_timer_start(scan_result_handler, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_UPDATE_CONN_PARA)
	{
		bsp_timer_start(conn_para_update_timerout_handler, time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_ADV_CONNECT_REQUEST)
	{
		//extern int adv_connect_request_timeout();
		//bsp_timer_start(adv_connect_request_timeout,time_ms_cnt*1000);
		return 0;
	}
	else if(timer_id==TIMER_CONNECT_MONITOR)
	{
		bsp_timer_start(tuya_conncet_monitor_callback,time_ms_cnt*1000);

		return 0;
	}
//	else if(timer_id==TIMER_SCAN)
//	{
//		tuya_user_timer_start(blt_soft_timer_scan_timeout_handler, time_ms_cnt*1000);
//		return 0;
//	}
	else if(timer_id==TIMER_IIC)
	{
		bsp_timer_start(tuya_SHT30_callback,time_ms_cnt*1000);
		return 0;
	}

    return 1;
}

u32 tuya_timer_delete(u8 timer_id)
{
	if(timer_id >= TIMER_ID_MAX)
	{
		return 1;
	}

	if(timer_id==TIMER_BLEDATA_SEND)
	{
		bsp_timer_delete(blt_soft_timer_bleData_send_handler);
		return 0;
	}
	else if(timer_id==TIMER_MCU_HEARTBEAT)
	{
		//extern s32 mcu_heartbeat_callback();
		//bsp_timer_delete(mcu_heartbeat_callback);
		return 0;
	}
	else if(timer_id==TIMER_REBOOT)
	{
		bsp_timer_delete(chip_reboot);
		return 0;
	}
	else if(timer_id==TIMER_RTC)
	{
		extern s32tuya_rtc_timer_update_handler();
		bsp_timer_delete(tuya_rtc_timer_update_handler);
		return 0;
	}
	else if(timer_id==TIMER_FIRST)
	{
		bsp_timer_delete(first_timerout_handler);
		return 0;
	}
	else if(timer_id==TIMER_FLASH_DATA_REPORT)
	{
		//extern s32 tuya_data_update_handler(void);
		//bsp_timer_delete(tuya_data_update_handler);
		return 0;
	}
	else if(timer_id==TIMER_OTA_TIMEOUT)
	{
		extern s32 ota_timeout_handler();
		bsp_timer_delete(ota_timeout_handler);
		return 0;
	}
	else if(timer_id==TIMER_OTA_END)
	{
			extern s32 ota_end_handler();
			bsp_timer_delete(ota_end_handler);
			return 0;
	}
	else if(timer_id==TIMER_UART_RX_TIMEOUT)
	{
		extern s32 uart_timeout_handler(void);
		bsp_timer_delete(uart_timeout_handler);
		return 0;
	}
	else if(timer_id==TIMER_SCAN)
	{
		extern int scan_timeout_handler();
		bsp_timer_delete(scan_timeout_handler);
		return 0;
	}
	else if(timer_id==TIMER_SCAN_RESULT)
	{
		extern int scan_result_handler();
		bsp_timer_delete(scan_result_handler);
		return 0;
	}
	else if(timer_id==TIMER_UPDATE_CONN_PARA)
	{
		bsp_timer_delete(conn_para_update_timerout_handler);
		return 0;
	}
	else if(timer_id==TIMER_ADV_CONNECT_REQUEST)
	{
		//extern int adv_connect_request_timeout();
		//bsp_timer_delete(adv_connect_request_timeout);
		return 0;
	}
	else if(timer_id==TIMER_CONNECT_MONITOR)
	{
		//extern  void tuya_ble_vtimer_conncet_monitor_callback(void* timer);
		bsp_timer_delete(tuya_conncet_monitor_callback);

		return 0;
	}
	else if(timer_id==TIMER_IIC)
	{
		return 0;
	}

    return 1;
}

void tuya_timer_start_conncet_monitor(u32 time_ms_cnt)
{
	tuya_timer_start(TIMER_CONNECT_MONITOR,time_ms_cnt);
}
void tuya_timer_stop_conncet_monitor()
{
	tuya_timer_delete(TIMER_CONNECT_MONITOR);
}

void tuya_reboot_timer_start(u32 time_ms)
{
	tuya_timer_start(TIMER_REBOOT,time_ms);
}

MYFIFO_INIT(ble_tx_fifo, 8, 32);

u32 tlsr_bleData_send(unsigned char *buf,unsigned int len)
{
    u8 *p = my_fifo_wptr (&ble_tx_fifo);
    u8* ble_evt_buffer=0;
    if (!p)
    {
    	tuya_log_d("tlsr_bleData_send error1");
        return TUYA_BLE_ERR_NO_MEM;
    }
    if((ble_evt_buffer=((u8 *)tuya_ble_malloc(len)))==0)
    {
    	tuya_log_d("tlsr_bleData_send error2");
    	return TUYA_BLE_ERR_NO_MEM;
    }

    memcpy(ble_evt_buffer,buf,len);

    p[0] = len;
    p[1] = len>>8;
    p[2] = ((u32)ble_evt_buffer)>>24;//鐎涙ɑ鏂侀幐鍥嫛閸︽澘娼�
    p[3] = ((u32)ble_evt_buffer)>>16;
    p[4] = ((u32)ble_evt_buffer)>>8;
    p[5] = ((u32)ble_evt_buffer)>>0;

    tuya_log_d("bleData_send:0x%08x-%d",ble_evt_buffer,len);
   // tuya_log_dumpHex("bleData_data:",len,buf,len);
    my_fifo_next (&ble_tx_fifo);
    return 0;
}




void ble_tx_exe0()
{
    u8 *p = my_fifo_get (&ble_tx_fifo);
//    if(p && ty_ble_send_flag)
//    {
//    	 tuya_log_d("ble_tx_exe busy");
//    }
    if (p && !ty_ble_send_flag)
    {
    	ble_send_len=p[0]+p[1]*256;
    	if(ble_send_len<=sizeof(ble_send_buf))
    	{
			memcpy(ble_send_buf,p+2,ble_send_len);
			if(tuya_timer_start(TIMER_BLEDATA_SEND,1)==0)
			{
				ty_ble_send_flag=1;
			}
			//tuya_log_d("ble_tx_exe ok-%d-%d",ble_send_len,err_code);
    	}
    	else
    	{
    		tuya_log_d("ble_tx_exe err-%d",ble_send_len);
    	}
    	//tuya_log_d("ble_tx_exe-%d-%d-%d",ble_send_len,ble_tx_fifo.wptr,ble_tx_fifo.rptr);
    	my_fifo_pop (&ble_tx_fifo);
    }
}

void ble_tx_exe()
{
    u8 *p = my_fifo_get (&ble_tx_fifo);
    u8* ble_buffer=0;
    u32 addr=0;
    if (p && !ty_ble_send_flag)
    {
    	ble_send_len=p[0]+p[1]*256;
    	addr=(p[2]<<24)+(p[3]<<16)+(p[4]<<8)+(p[5]<<0);
    	ble_buffer=(u8*)addr;
    	if(ble_send_len<=sizeof(ble_send_buf))
    	{
			memcpy(ble_send_buf,ble_buffer,ble_send_len);
			if(tuya_timer_start(TIMER_BLEDATA_SEND,1)==0)
			{
				ty_ble_send_flag=1;
			}
			//tuya_log_d("ble_tx_exe ok-%d-%d",ble_send_len,err_code);
    	}
    	else
    	{
    		//tuya_log_d("ble_tx_exe err-%d",ble_send_len);
    	}
    	tuya_log_d("ble_tx_exe:%d-0x%08x-%d",sizeof(ble_send_buf),ble_buffer,ble_send_len);
    	tuya_ble_free(ble_buffer);
    	my_fifo_pop (&ble_tx_fifo);
    }
}

unsigned char ble_tx_is_busy()
{
	if((my_fifo_get (&ble_tx_fifo))||ty_ble_send_flag)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



void tuya_print_sysInfor()
{
	//tuya_log_d("firmName:%s,firmVer:%s,sdkVer:3.1",TY_FIRMWARE_NAME,TY_FIRMWARE_VER_STR);
    tuya_log_dumpHex("mac", MAC_LEN, tuya_ble_current_para.auth_settings.mac, MAC_LEN);
    tuya_log_dumpHex("product_id", PRODUCT_ID_LEN, tuya_ble_current_para.pid, PRODUCT_ID_LEN);
    tuya_log_dumpHex("device_id ", 16, tuya_ble_current_para.auth_settings.device_id, DEVICE_ID_LEN);
    tuya_log_dumpHex("auth_key  ", 16, tuya_ble_current_para.auth_settings.auth_key, AUTH_KEY_LEN);
    tuya_log_dumpHex("login_key ", 16, tuya_ble_current_para.sys_settings.login_key, LOGIN_KEY_LEN);
    tuya_log_dumpHex("virtual_id ", DEVICE_VIRTUAL_ID_LEN, tuya_ble_current_para.sys_settings.device_virtual_id, DEVICE_VIRTUAL_ID_LEN);
    tuya_log_d("bond_flag:%d ",tuya_ble_current_para.sys_settings.bound_flag);
}

void ty_set_adv_enable(u8 en)
{
    //enable & disable
    bls_ll_setAdvEnable(en);
}
u32 get_sysrun_ms()
{
	return (clock_time()/CLOCK_SYS_CLOCK_1MS);
}

u32 tuya_rtc_get_timestamp()
{
	return unix_time;
}



