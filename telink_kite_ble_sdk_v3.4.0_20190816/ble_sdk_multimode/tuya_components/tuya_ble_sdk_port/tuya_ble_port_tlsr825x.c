//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

#include "tuya_ble_port.h"
#include "tuya_ble_common.h"
//#include "tuya_ble_internal_config.h"

#include "aes.h"
#include "md5.h"


tuya_ble_status_t tuya_ble_gap_advertising_adv_data_update(uint8_t const* p_ad_data, uint8_t ad_len)
{
    //update_adv_data(p_ad_data,ad_len);
	 bls_ll_setAdvData(p_ad_data,ad_len);
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_gap_advertising_scan_rsp_data_update(uint8_t const *p_sr_data, uint8_t sr_len)
{
    //update_scan_rsp_data(p_sr_data,sr_len);
	bls_ll_setScanRspData(p_sr_data,sr_len);
    return TUYA_BLE_SUCCESS;
}



tuya_ble_status_t tuya_ble_gap_disconnect(void)
{
    // ble_device_disconnected();
	if(blc_ll_getCurrentState()==BLS_LINK_STATE_CONN)
	{
		bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
	}
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_gap_address_get(uint8_t mac[6])
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gap_addr_set(tuya_ble_gap_addr_t *p_addr)
{
    hal_bt_set_mac(p_addr->addr);

    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gatt_send_data(const uint8_t *p_data,uint16_t len)
{
    uint8_t data_len = len;
    if(data_len>TUYA_BLE_DATA_MTU_MAX)
    {
        data_len = TUYA_BLE_DATA_MTU_MAX;
    }

	if(bls_ll_isConnectState())
	{
		return bls_att_pushNotifyData(0x11, p_data, len);
	}
    else
    {
        return TUYA_BLE_ERR_INVALID_STATE;
    }
}

tuya_ble_status_t tuya_ble_timer_create(void** p_timer_id,uint32_t timeout_value_ms, tuya_ble_timer_mode mode,tuya_ble_timer_handler_t timeout_handler)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_timer_delete(void* timer_id)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_timer_start(void* timer_id)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_timer_stop(void* timer_id)
{
    return TUYA_BLE_SUCCESS;
}


void tuya_ble_device_delay_ms(uint32_t ms)
{
   // nrf_delay_ms(ms);
}


tuya_ble_status_t tuya_ble_rand_generator(uint8_t* p_buf, uint8_t len)
{

     for(uint32_t i=0; i<len; i++)
    {
    	 if(i==0) p_buf[i]=5;
    	 else
    	 {
    		 p_buf[i] = rand();
    	 }
    }
    return TUYA_BLE_SUCCESS;
}

/*
 *@brief
 *@param
 *
 *@note
 *
 * */
tuya_ble_status_t tuya_ble_device_reset(void)
{
    //NVIC_SystemReset();
    return TUYA_BLE_SUCCESS;
}

unsigned char intState=0;
void tuya_ble_device_enter_critical(void)
{
    intState=irq_disable();

}

void tuya_ble_device_exit_critical(void)
{
    irq_restore(intState);
}


tuya_ble_status_t tuya_ble_rtc_get_timestamp(uint32_t *timestamp,int32_t *timezone)
{
    *timestamp = 0;
    *timezone = 0;
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_rtc_set_timestamp(uint32_t timestamp,int32_t timezone)
{

    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_nv_init(void)
{
    //nrf_fstorage_port_init();
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_nv_erase(uint32_t addr,uint32_t size)
{
    tuya_ble_status_t result = TUYA_BLE_SUCCESS;

    uint32_t erase_pages, i;

    /* make sure the start address is a multiple of FLASH_ERASE_MIN_SIZE */
    if(addr % TUYA_NV_ERASE_MIN_SIZE != 0)
    {
       // app_log_d("the start address is a not multiple of TUYA_NV_ERASE_MIN_SIZE");
        return TUYA_BLE_ERR_INVALID_ADDR;
    }

    /* calculate pages */
    erase_pages = size / TUYA_NV_ERASE_MIN_SIZE;
    if (size % TUYA_NV_ERASE_MIN_SIZE != 0) {
        erase_pages++;
    }

    /* start erase */
    for (i = 0; i < erase_pages; i++)
	{
		//if(nrf_fstorage_port_erase_sector(addr + (4096 * i),true)!=0)
        flash_erase_sector(addr + (4096 * i));
//		{
//			result = TUYA_BLE_ERR_INTERNAL;
//            break;
//		}
    }
    return result;
}

tuya_ble_status_t tuya_ble_nv_write(uint32_t addr,const uint8_t *p_data, uint32_t size)
{
	//nv_flashWrite(addr,size, p_data);
	tuya_log_d("flash_write_page-0x%08x,%d",addr,size);
	flash_write_page(addr,size,p_data);//flash_write_page
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_nv_read(uint32_t addr,uint8_t *p_data, uint32_t size)
{
	flash_read_page(addr,size,p_data);
    return TUYA_BLE_SUCCESS;
}


#if TUYA_BLE_USE_OS

bool tuya_ble_os_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),void *p_param, uint16_t stack_size, uint16_t priority)
{
    return os_task_create(pp_handle, p_name, p_routine,p_param, stack_size, priority);
}

bool tuya_ble_os_task_delete(void *p_handle)
{
    return os_task_delete(p_handle);
}

bool tuya_ble_os_task_suspend(void *p_handle)
{
    return os_task_suspend(p_handle);
}

bool tuya_ble_os_task_resume(void *p_handle)
{
    return os_task_resume(p_handle);
}

bool tuya_ble_os_msg_queue_create(void **pp_handle, uint32_t msg_num, uint32_t msg_size)
{
    return os_msg_queue_create(pp_handle, msg_num, msg_size);
}

bool tuya_ble_os_msg_queue_delete(void *p_handle)
{
    return os_msg_queue_delete(p_handle);
}

bool tuya_ble_os_msg_queue_peek(void *p_handle, uint32_t *p_msg_num)
{
    return os_msg_queue_peek(p_handle, p_msg_num);
}

bool tuya_ble_os_msg_queue_send(void *p_handle, void *p_msg, uint32_t wait_ms)
{
    return os_msg_send(p_handle, p_msg, wait_ms);
}

bool tuya_ble_os_msg_queue_recv(void *p_handle, void *p_msg, uint32_t wait_ms)
{
    return os_msg_recv(p_handle, p_msg, wait_ms);
}

#endif


bool tuya_ble_aes128_ecb_encrypt(uint8_t *key,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    uint16_t length;
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    length = input_len;

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);

    while( length > 0 )
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, input, output );
        input  += 16;
        output += 16;
        length -= 16;
    }

    mbedtls_aes_free(&aes_ctx);

    return true;
}

bool tuya_ble_aes128_ecb_decrypt(uint8_t *key,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    uint16_t length;
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    length = input_len;

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);

    while( length > 0 )
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_DECRYPT, input, output );
        input  += 16;
        output += 16;
        length -= 16;
    }

    mbedtls_aes_free(&aes_ctx);

    return true;
}

bool tuya_ble_aes128_cbc_encrypt(uint8_t *key,uint8_t *iv,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);

    mbedtls_aes_crypt_cbc(&aes_ctx,MBEDTLS_AES_ENCRYPT,input_len,iv,input,output);
    //
    mbedtls_aes_free(&aes_ctx);

    return true;
}

bool tuya_ble_aes128_cbc_decrypt(uint8_t *key,uint8_t *iv,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);

    mbedtls_aes_crypt_cbc(&aes_ctx,MBEDTLS_AES_DECRYPT,input_len,iv,input,output);
    //
    mbedtls_aes_free(&aes_ctx);

    return true;
}


bool tuya_ble_md5_crypt(uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_md5_context md5_ctx;
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, input, input_len);
    mbedtls_md5_finish(&md5_ctx, output);
    mbedtls_md5_free(&md5_ctx);
	//tuya_log_dumpHex("md5-2:",100,output,16);
    return true;
}


void* ty_malloc(uint16_t size)
{
	extern void *tuya_ble_malloc(uint16_t size);
	return tuya_ble_malloc(size);
}

void ty_free(void* ptr)
{
	extern void tuya_ble_free(ptr);
	tuya_ble_free(ptr);
}

u32 tuya_bsp_flash_read_bytes(uint32_t addr,uint8_t *p_data, uint32_t size)
{
	return tuya_ble_nv_read(addr,p_data, size);
}

u32 tuya_bsp_flash_write_bytes(uint32_t addr,uint8_t *p_data, uint32_t size)
{
	return tuya_ble_nv_write(addr,p_data, size);
}
u32 tuya_bsp_flash_erease_sector(uint32_t addr)
{
	tuya_ble_nv_erase(addr,4096);
	return 0;
}

u32 tuya_bsp_uart_send_bytes(u8 * data,u16 len)
{
	return uart_send_data (data,len);
}

u32 tuya_bsp_log_send_bytes(u8 * data,u16 len)
{
	extern u32 log_send_bytes(u8*data,u16 data_len);
	return log_send_bytes(data,len);
}

tuya_ble_status_t tuya_ble_common_uart_send_data(const uint8_t *p_data,uint16_t len)
{
	uart_send_data (p_data,len);
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_common_uart_init(void)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_timer_restart(void* timer_id,uint32_t timeout_value_ms)
{
	return TUYA_BLE_SUCCESS;
}
