/*
 * tuya_rf_test.c
 *
 *  Created on: 2017-10-26
 *      Author: echo
 */

#include "tuya_rf_test.h"
#include "tuya_ble_common.h"
#include "tuya_utils.h"

static scan_rf_type_t scan_test_type = FACTORY_TEST;

//void tuya_ble_scan_callback(void *metaData)
void tuya_ble_scan_callback(u32 h, u8 *para, int n)

{
	event_adv_report_t *pa = (event_adv_report_t *)para;
	s8 rssi = pa->data[pa->len];

	data_t adv_data;
	data_t dev_name;
	adv_data.p_data = pa->data;
	adv_data.data_len = pa->len;

	//printf("adv_report_callback\n");
	//tuya_log_d("adv_report\n");
	if(0 == adv_report_parse(GAP_ADTYPE_LOCAL_NAME_COMPLETE,&adv_data,&dev_name))
	{
		//tuya_log_v("GAP_ADTYPE_LOCAL_NAME_COMPLETE");
		tuya_factorytest_on_scanrsp(dev_name.p_data,dev_name.data_len,rssi);
	}
	else
	{

	}
}

void ty_ble_scan_start(void)
{
	extern u8  tbl_mac [];
	 //scan setting
	bls_ll_setAdvEnable(1);  //adv enable

	//scan set
	blc_ll_initScanning_module(tbl_mac);		//scan module: 		 optional
	blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_ADVERTISING_REPORT);
	
	blc_hci_registerControllerEventHandler(tuya_ble_scan_callback);		//register event callback
	
	blc_ll_setScanParameter(SCAN_TYPE_PASSIVE, SCAN_INTERVAL_90MS, SCAN_INTERVAL_90MS,
									  OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
	blc_ll_addScanningInAdvState();  //add scan in adv state
	blc_ll_addScanningInConnSlaveRole();  //add scan in conn slave role
	blc_ll_setScanEnable (1, 0);
}

void ty_ble_scan_stop(void)
{
	//bls_ll_setAdvEnable(1);
	//blc_ll_setScanEnable (0, 0);
	blc_ll_setScanEnable (0, 0);
	blc_ll_removeScanningFromAdvState();
	blc_ll_removeScanningFromConnSLaveRole();
	extern int controller_event_handler(u32 h, u8 *para, int n);
	blc_hci_registerControllerEventHandler(controller_event_handler);		//register event callback
}


int scan_timeout_handler()
{
	s8 rssi=0;
	tuya_log_d("scan_timeout_handler");
	ty_ble_scan_stop();
	rssi= tuya_auth_rf_test_calc_avg();
	tuya_test_rf_resp(0,rssi);
	tuya_auth_rf_test_begin_stats_flag_set(0);
	return -1;
}
int scan_result_handler()
{
	ty_ble_scan_stop();
	s8 rssi = tuya_auth_rf_test_calc_avg();
	tuya_test_rf_resp(1,rssi);
	tuya_auth_rf_test_begin_stats_flag_set(0);
	return -1;
}
////////////////////////////////////////fac_test_cmd_resp api////////////////////////////////////////////
static char* itoa(int num,char* str,int radix)
{/*缁便垹绱╃悰锟�*/
    char index[]="0123456789ABCDEF";
    unsigned unum;/*娑擃參妫块崣姗�鍣�*/
    int i=0,j,k;
    /*绾喖鐣緐num閻ㄥ嫬锟斤拷*/
    if(radix==10&&num<0)/*閸椾浇绻橀崚鎯扮閺侊拷*/
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;/*閸忔湹绮幆鍛枌*/
    /*鏉烆剚宕�*/
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
       }while(unum);
    str[i]='\0';
    /*闁棗绨�*/
    if(str[0]=='-')
        k=1;/*閸椾浇绻橀崚鎯扮閺侊拷*/
    else
        k=0;
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}

void tuya_factory_test_auth_rf_resp(unsigned char suc_flag,char rssi)
{
	u8 alloc_buf[60];
	u8 str[10];
	u8 i;
	//alloc_buf [60];
	memset(alloc_buf,0x0,60);
	rssi = ~(rssi - 1);//dev_rssi;
	int rssie = -rssi;
	if(suc_flag)
	{
		//sprintf((char *)alloc_buf,"{\"ret\":true,\"rssi\":\"%d\"}",rssie);
		i = 0;
		//alloc_buf = ty_malloc(60);
		alloc_buf[i++] = '{';
		alloc_buf[i++] = '\"';
		memcpy(&alloc_buf[i],"ret",3);
		i+=3;
		alloc_buf[i++] = '\"';

		alloc_buf[i++] = ':';
		memcpy(&alloc_buf[i],"true",4);
		i+=4;
		alloc_buf[i++] = ',';
		alloc_buf[i++] = '\"';
		memcpy(&alloc_buf[i],"rssi",4);
		i+=4;
		alloc_buf[i++] = '\"';
		alloc_buf[i++] = ':';
		alloc_buf[i++] = '\"';
		itoa(rssie,str,10);
		memcpy(&alloc_buf[i],str,3);
		i+=3;
		alloc_buf[i++] = '\"';
		alloc_buf[i++] = '}';
		ty_uart_protocol_factory_send(0x08, alloc_buf,i);
	}
	else
	{
		//sprintf((char *)alloc_buf,"{\"ret\":false}");
		u8 buf[] = "{\"ret\":rssi false}";
        //tuya_log_d("\nrssi flase\n");
        ty_uart_protocol_factory_send(0x08,buf,strlen(buf));
	}
	tuya_log_v("%s,%d",alloc_buf,suc_flag);
	
	///ty_free(alloc_buf);
}

void tuya_factory_test_factory_rf_resp(unsigned char suc_flag,char rssi)
{
	u8 alloc_buf[60];
	//alloc_buf = ty_malloc(60);
	memset(alloc_buf,0x0,60);
	rssi = ~(rssi - 1);//dev_rssi;
	int rssie = -rssi;

	if(suc_flag)
	{
		sprintf((char *)alloc_buf,"{\"ret\":true,\"rssi\":\"%d\"}",rssie);
	}
	else
	{
		sprintf((char *)alloc_buf,"{\"ret\":false}");
	}

	ty_uart_protocol_send(TY_SCAN_RSSI, alloc_buf,strlen((char *)alloc_buf));
	//ty_free(alloc_buf);
}

void tuya_test_rf_resp(unsigned char suc_flag,char rssi)
{

	scan_rf_type_t type = tuya_factory_test_scan_type_get();
	tuya_log_v("rssi_avg:%d",(signed int)rssi);
	if(type == FACTORY_TEST)
	{
		tuya_log_v("tuya_factory_test_factory_rf_resp");
		tuya_factory_test_factory_rf_resp(suc_flag,rssi);
	}
	else
	{
		tuya_log_v("tuya_factory_test_auth_rf_resp");
		tuya_factory_test_auth_rf_resp(suc_flag,rssi);
	}//factory test
}


////////////////////////////////dev scan static/////////////////////////////////

static data_stats_t auth_rf_test_rssi_stats;

static unsigned char auth_rf_test_begin_stats_flag = 0;

unsigned char tuya_auth_rf_test_begin_stats_flag_get(void)
{
	return auth_rf_test_begin_stats_flag;
}

void tuya_auth_rf_test_begin_stats_flag_set(unsigned char flag)
{
	auth_rf_test_begin_stats_flag = flag;
}

void tuya_auth_rf_test_rssi_stats_add(char rssi)
{
	if( auth_rf_test_rssi_stats.index >= MAX_BUFFER_DATA )
	{
		s8 old_avg = tuya_auth_rf_test_calc_avg();
		tuya_auth_rf_test_rssi_stats_reset();

		auth_rf_test_rssi_stats.data[0] = old_avg;
	}
	auth_rf_test_rssi_stats.data[auth_rf_test_rssi_stats.index++] = (u8)rssi;
	tuya_log_v("rssi:%d",rssi);
}

void tuya_auth_rf_test_rssi_stats_reset(void)
{
	memset(&auth_rf_test_rssi_stats,0x0,sizeof(data_stats_t));//reset stats data
}



////////////////////////////////dev scan static end/////////////////////////////////



////////////////////////////////////////dev scan api/////////////////////////////////////////////////////

#define FACTORY_TEST_BEACON_NAME		"ty_mdev"
#define AUTH_RF_TEST_BEACON_NAME		"ty_prod"



scan_rf_type_t tuya_factory_test_scan_type_get(void)
{
	return scan_test_type;
}


char tuya_auth_rf_test_calc_avg(void)
{
	s16 sum = 0;
	u8 i = 0;
	for(i = 0 ; i < auth_rf_test_rssi_stats.length ; i++)
	{
		sum += (s8)(auth_rf_test_rssi_stats.data[i]);
	}

	return (s8)(sum/(auth_rf_test_rssi_stats.length));
}
//NEED refator???

//static tuya_factory_test_cb_fun cb_fun = NULL;

void tuya_factorytest_on_scanrsp(unsigned char *dev_name, unsigned char  dev_name_len, char dev_rssi)
{

	if(!clock_time_exceed(0,100*1000))
	{
		return;
	}
    char adv_name[7];
	u8 tp_buf[32]={0};
	memcpy(tp_buf,dev_name,dev_name_len);

	//int rssi = -((((u16)-1)-dev_rssi)+1);
	dev_rssi = ~(dev_rssi - 1);//dev_rssi;
	int rssi = -dev_rssi;

	/////////////////////////////
	if(strlen(tp_buf) > 1)
	{
		//tuya_log_d("dev_name:%s,\tdev_rssi:%d\r\n",tp_buf,rssi);
	}

	/////////////////////////////

	scan_rf_type_t type = tuya_factory_test_scan_type_get();

	if(type == FACTORY_TEST)//factory test
	{
		memcpy(adv_name,FACTORY_TEST_BEACON_NAME,7);
	}
	else if(type == AUTH_RF_TEST)//auth rf test
	{
		memcpy(adv_name,AUTH_RF_TEST_BEACON_NAME,7);
	}
	else
	{

	}
	//tuya_log_v("rf_test_rssi");
	if((memcmp(dev_name,adv_name,7) == 0))
	{
		//tuya_log_v("tuya_auth_rf_test_rssi_stats_add");
		if(tuya_auth_rf_test_begin_stats_flag_get())
		{
			tuya_auth_rf_test_rssi_stats_add(rssi);
		}
		else
		{
			//begin calc
			tuya_auth_rf_test_begin_stats_flag_set(TRUE);

			tuya_auth_rf_test_rssi_stats_reset();

			tuya_timer_delete(TIMER_SCAN);//cancel scan name timeout

			tuya_timer_start(TIMER_SCAN_RESULT,1000);//continue scan 1s

			tuya_auth_rf_test_rssi_stats_add(rssi);//add frist one
		}
	}
}

//tuya_factory_test_cb_fun tuya_factory_test_get_cb(void)
//{
//	return cb_fun;
//}

//void tuya_user_app_regist_factory_test_cb(tuya_factory_test_cb_fun cb)
//{
//	cb_fun = cb;
//}

void tuya_start_factory_test_scan(scan_rf_type_t test_type,u32 timeout_ms)
{
	scan_test_type = test_type;
	ty_ble_scan_stop();
	ty_ble_scan_start();
	tuya_timer_start(TIMER_SCAN,timeout_ms); //scan time out 5s
}



