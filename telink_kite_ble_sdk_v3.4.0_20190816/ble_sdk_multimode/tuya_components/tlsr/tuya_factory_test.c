/*
 * tuya_factory_test.c
 *
 *  Created on: 2017-9-6
 *      Author: echo
 */
#include "tuya_ble_common.h"

#include "tuya_ble_app_demo.h"

#define TY_FIRMWARE_NAME     APP_BUILD_FIRMNAME
#define TY_FIRMWARE_VER_STR  TY_APP_VER_STR

#define TUYA_LOG(...)


void tuya_uart_factory_test(u8 *pData,u16 len)
{
	static u8 if_enter = 0;
    u8 i=0,temp=0;
    u8 alloc_buf[300];

    tuya_log_dumpHex("tuya_uart_factory_test:",len,pData,len);
    u8 mac[13];
	u16 data_len=0;
	data_len=(pData[4]<<8)|(pData[5]<<0);

    	if((0x00 == pData[3])&&(data_len==0))
    	{
    		if(if_enter!=2)
    		{
				tuya_timer_delete(TIMER_FIRST);
				//uart_heartbeat_timer_delete();
				temp=0x01;
				ty_uart_protocol_factory_send(0x00, &temp,1);
				if_enter = 1;
				bls_ll_setAdvEnable(0);
				u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS+32, \
												   ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
													0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
				bls_ll_setAdvEnable(1);
			}
    	}
    	else if(0!= if_enter)
    	{
			switch(pData[3])
			{
				case 0x01://read hid
					if_enter=2;
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_H_ID, &alloc_buf[40], H_ID_LEN);
					memcpy(&alloc_buf[40],tuya_ble_current_para.auth_settings.h_id,H_ID_LEN+1);

					//alloc_buf[40] = 0;//my add
					if(0x01 == alloc_buf[40]){
						alloc_buf[0] = '{';
						alloc_buf[1] = '\"';
						memcpy(&alloc_buf[2],"ret",3);
						alloc_buf[5] = '\"';

						alloc_buf[6] = ':';
						memcpy(&alloc_buf[7],"true",4);

						alloc_buf[11] = ',';
						alloc_buf[12] = '\"';
						memcpy(&alloc_buf[13],"hid",3);
						alloc_buf[16] = '\"';
						alloc_buf[17] = ':';
						alloc_buf[18] = '\"';

						memcpy(&alloc_buf[19],&alloc_buf[41],19);
						alloc_buf[38] = '\"';
						alloc_buf[39] = '}';
						ty_uart_protocol_factory_send(0x01, alloc_buf,40);
					}
					else
					{
						alloc_buf[0] = '{';
						alloc_buf[1] = '\"';
						memcpy(&alloc_buf[2],"ret",3);
						alloc_buf[5] = '\"';

						alloc_buf[6] = ':';
						memcpy(&alloc_buf[7],"true",4);

						alloc_buf[11] = ',';
						alloc_buf[12] = '\"';
						memcpy(&alloc_buf[13],"hid",3);
						alloc_buf[16] = '\"';
						alloc_buf[17] = ':';
						alloc_buf[18] = '\"';
						alloc_buf[19] = '\"';
						alloc_buf[20] = '}';
						ty_uart_protocol_factory_send(0x01, alloc_buf,21);
					}
					//ty_free(alloc_buf);
				break;

				case 0x02:
					if_enter=2;
					if(drv_gpio_base_test_auto() == 1)
					{
				        u8 buf[] = "{\"ret\":true}";
				        ty_uart_protocol_factory_send(0x02,buf,strlen(buf));
				    }
			        else
				    {
				        u8 buf[] = "{\"ret\":false}";
				        ty_uart_protocol_factory_send(0x02,buf,strlen(buf));
				    }
					#if 0
					//alloc_buf = ty_malloc(100);
					//gpio_test_run(&alloc_buf[50]);
					alloc_buf[0] = '{';
					alloc_buf[1] = '\"';
					memcpy(&alloc_buf[2],"ret",3);
					alloc_buf[5] = '\"';
					alloc_buf[6] = ':';
#if SKIP_GPIO_TEST
					alloc_buf[50] = 0;//
#endif
					if(0 == alloc_buf[50]){
						memcpy(&alloc_buf[7],"true",4);
						alloc_buf[11] = '}';
						ty_uart_protocol_factory_send(0x02, alloc_buf,12);
					}
					else{
						memcpy(&alloc_buf[7],"false",5);
						alloc_buf[12] = '}';
						ty_uart_protocol_factory_send(0x02, alloc_buf,13);
					}
					//ty_free(alloc_buf);
					#endif
				break;

				case 0x03://閸愭瑥鍙嗛幒鍫熸綀閻拷
					//tuya
					if_enter=2;
					//alloc_buf = ty_malloc(200);
					memcpy(&alloc_buf[60],&pData[59],16);
					if(0 == memcmp(&pData[87+23],"true",4))
					{//+24 for mac offset
						alloc_buf[76] = 0x01;
					}
					else if(0 == memcmp(&pData[87+23],"false",5))
					{
						alloc_buf[76] = 0x0;
					}

					//NV_USER_ITEM_SAVE(NV_USER_ITEM_AUZ_KEY, &pData[17], AUTH_KEY_LEN);
					//NV_USER_ITEM_SAVE(NV_USER_ITEM_D_ID, &alloc_buf[60], D_ID_LEN+1);
					//NV_USER_ITEM_SAVE(NV_USER_ITEM_MAC, &pData[84], MAC_LEN);
					memcpy(tuya_ble_current_para.auth_settings.auth_key,&pData[17],AUTH_KEY_LEN);
					memcpy(tuya_ble_current_para.auth_settings.device_id,&alloc_buf[60],D_ID_LEN);
					memcpy(tuya_ble_current_para.auth_settings.mac_string,&pData[84],MAC_LEN*2);
					hexstr2hex(&pData[84],12,mac);
					memcpy(tuya_ble_current_para.auth_settings.mac,mac,MAC_LEN);
					
					tuya_ble_storage_save_auth_settings();

					if(tuya_ble_nv_erase(CFG_ADR_MAC,TUYA_NV_ERASE_MIN_SIZE)==TUYA_BLE_SUCCESS)
    				{		
						tuya_ble_nv_write(CFG_ADR_MAC,(uint8_t *)tuya_ble_current_para.auth_settings.mac,MAC_LEN);
					}
					tuya_ble_storage_load_settings();

					//NV_USER_ITEM_LOAD(NV_USER_ITEM_AUZ_KEY, alloc_buf, AUTH_KEY_LEN);
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_D_ID, &alloc_buf[40], D_ID_LEN);
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_MAC, &alloc_buf[80], MAC_LEN);
					memcpy(alloc_buf,tuya_ble_current_para.auth_settings.auth_key,AUTH_KEY_LEN);
					memcpy(&alloc_buf[40],tuya_ble_current_para.auth_settings.device_id,D_ID_LEN);
					memcpy(&alloc_buf[80],tuya_ble_current_para.auth_settings.mac,MAC_LEN);

					if((0 == memcmp(alloc_buf,&pData[17],AUTH_KEY_LEN))&&
						(0 == memcmp(&alloc_buf[40],&pData[59],D_ID_LEN))&&
						    (0 == memcmp(&alloc_buf[80],mac,MAC_LEN)))
					{
						alloc_buf[0] = '{';
						alloc_buf[1] = '\"';
						memcpy(&alloc_buf[2],"ret",3);
						alloc_buf[5] = '\"';

						alloc_buf[6] = ':';
						memcpy(&alloc_buf[7],"true",4);
						alloc_buf[11] = '}';

//						//write mac
//						if(hexstr2hex(&pData[84],MAC_LEN,&alloc_buf[80]) == 1){
//							flash_erase_sector(CFG_MAC_ADDRESS);
//							flash_write_page(CFG_MAC_ADDRESS, 6 , &alloc_buf[80]);
//						}

						ty_uart_protocol_factory_send(0x03, alloc_buf,12);
					}
					else
					{
						alloc_buf[0] = '{';
						alloc_buf[1] = '\"';
						memcpy(&alloc_buf[2],"ret",3);
						alloc_buf[5] = '\"';

						alloc_buf[6] = ':';
						memcpy(&alloc_buf[7],"false",5);
						alloc_buf[12] = '}';
						ty_uart_protocol_factory_send(0x03, alloc_buf,13);
					}
					//ty_free(alloc_buf);
				break;

				case 0x04://read
					if_enter=2;
					i = 0;
					//alloc_buf = ty_malloc(250);
					alloc_buf[i++] = 0x66;
					alloc_buf[i++] = 0xaa;
					alloc_buf[i++] = 0x0;
					alloc_buf[i++] = 0x04;//type;
					alloc_buf[i++] = 0;
					i++;//len
					alloc_buf[i++] = '{';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"ret",3);
					i += 3;
					alloc_buf[i++] = '\"';

					alloc_buf[i++] = ':';
					memcpy(&alloc_buf[i],"true",4);
					i += 4;
					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"auzKey",6);
					i += 6;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_AUZ_KEY, &alloc_buf[i], AUTH_KEY_LEN);
					memcpy(&alloc_buf[i],tuya_ble_current_para.auth_settings.auth_key,AUTH_KEY_LEN);
					i += AUTH_KEY_LEN;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"hid",3);
					i += 3;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_H_ID, &alloc_buf[100], H_ID_LEN);
					memcpy(&alloc_buf[100],tuya_ble_current_para.auth_settings.h_id,H_ID_LEN+1);
					temp = 0;//alloc_buf[100];
					memcpy(&alloc_buf[i],&alloc_buf[101],19);
					i += 19;
					alloc_buf[i++] = '\"';

					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"uuid",4);
					i += 4;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_D_ID, &alloc_buf[i], D_ID_LEN+1);
					memcpy( &alloc_buf[i],tuya_ble_current_para.auth_settings.device_id,D_ID_LEN);
					i += D_ID_LEN;
					//temp = alloc_buf[i];
					alloc_buf[i++] = '\"';

					//add mac
					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"mac",3);
					i += 3;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_MAC, &alloc_buf[i], MAC_LEN);
					//hex2hexstr(tuya_ble_current_para.auth_settings.mac, 6,mac);
					memcpy(&alloc_buf[i],tuya_ble_current_para.auth_settings.mac_string,12);

					i += 12;
					alloc_buf[i++] = '\"';

					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"firmName",8);
					i += 8;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],TY_FIRMWARE_NAME,strlen(TY_FIRMWARE_NAME));
					i+=strlen(TY_FIRMWARE_NAME);
					alloc_buf[i++] = '\"';

					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"firmVer",7);
					i+=7;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],TY_FIRMWARE_VER_STR,strlen((char *)TY_FIRMWARE_VER_STR));
					i+=strlen((char *)TY_FIRMWARE_VER_STR);
					alloc_buf[i++] = '\"';

					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"prod_test",9);
					i+=9;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					if(0x1 == temp){
						memcpy(&alloc_buf[i],"true",4);
						i += 4;
					}
					else if(0x0 == temp){
						memcpy(&alloc_buf[i],"false",5);
						i += 5;
					}

					alloc_buf[i++] = '}';
					alloc_buf[5] = i-6;
					alloc_buf[i] = check_sum(alloc_buf,i);
					i++;
					tuya_uart_common_send_bytes(alloc_buf,i);
					//ty_free(alloc_buf);
				break;

				case 0x05://reset
					if_enter=2;
					ty_uart_protocol_factory_send(0x05,NULL,0);
					tuya_reboot_timer_start(100);
				break;

				case 0x06:
					if_enter=2;
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
					memcpy(&alloc_buf[i],"firmName",8);
					i+=8;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],TY_FIRMWARE_NAME,strlen(TY_FIRMWARE_NAME));
					i+=strlen(TY_FIRMWARE_NAME);
					alloc_buf[i++] = '\"';

					alloc_buf[i++] = ',';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],"firmVer",7);
					i+=7;
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = ':';
					alloc_buf[i++] = '\"';
					memcpy(&alloc_buf[i],TY_FIRMWARE_VER_STR,strlen((char *)TY_FIRMWARE_VER_STR));
					i+=strlen((char *)TY_FIRMWARE_VER_STR);
					alloc_buf[i++] = '\"';
					alloc_buf[i++] = '}';
					ty_uart_protocol_factory_send(0x06, alloc_buf,i);
					//ty_free(alloc_buf);
				break;

				case 0x07://write hid
					if_enter=2;
					//alloc_buf = ty_malloc(50);
					memcpy(&alloc_buf[1],&pData[14],19);
					alloc_buf[0] = 0x01;
					//NV_USER_ITEM_SAVE(NV_USER_ITEM_H_ID, alloc_buf, H_ID_LEN);
					//NV_USER_ITEM_LOAD(NV_USER_ITEM_H_ID, &alloc_buf[30], H_ID_LEN);
					memcpy(tuya_ble_current_para.auth_settings.h_id,alloc_buf,H_ID_LEN+1);
					tuya_ble_storage_save_auth_settings();
					tuya_ble_storage_load_settings();
					memcpy(&alloc_buf[30],tuya_ble_current_para.auth_settings.h_id,H_ID_LEN+1);

					if(0 == memcmp(&alloc_buf[30],alloc_buf,H_ID_LEN))
					{
						//save 76000 for mac
						//flash_erase_sector(CFG_MAC_ADDRESS);
						memcpy(alloc_buf,&pData[21],12);
						for(i =0;i<6;i++)
						{
							alloc_buf[30+i] = alloc_buf[10-i*2] - 0x30;
							alloc_buf[30+i] <<=4;
							alloc_buf[30+i] += (alloc_buf[11-i*2] - 0x30);
						}
						//nv_flashWrite(CFG_MAC_ADDRESS,6,&alloc_buf[30]);

						alloc_buf[0] = '{';
						alloc_buf[1] = '\"';
						memcpy(&alloc_buf[2],"ret",3);
						alloc_buf[5] = '\"';

						alloc_buf[6] = ':';
						memcpy(&alloc_buf[7],"true",4);
						alloc_buf[11] = '}';
						ty_uart_protocol_factory_send(0x07, alloc_buf,12);
					}
					else
					{
						alloc_buf[0] = '{';
						alloc_buf[1] = '\"';
						memcpy(&alloc_buf[2],"ret",3);
						alloc_buf[5] = '\"';

						alloc_buf[6] = ':';
						memcpy(&alloc_buf[7],"false",5);
						alloc_buf[12] = '}';
						ty_uart_protocol_factory_send(0x07, alloc_buf,13);
					}
					//ty_free(alloc_buf);
				break;
				case 0x08://rf test
					if_enter=2;
					//閻庢鍠栭崐褰掝敆閻愬搫绠规繝濠傛噹閸嬪秵顨ラ悙鑸电【婵炲弶鐗楀鍕晸閿燂拷
					//TUYA_LOG("rf test\n");
					tuya_start_factory_test_scan(AUTH_RF_TEST,3000);
					break;
				default:
					break;
			}
    	}
}

void tuya_ble_app_production_test_process(uint8_t channel,uint8_t *p_in_data,uint16_t in_len)
{

}
void tuya_ble_internal_production_test_with_ble_flag_clear(void)
{

}


