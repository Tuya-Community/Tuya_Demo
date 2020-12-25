/*
 * tuya_rf_test.h
 *
 *  Created on: 2017-10-26
 *      Author: echo
 */

#ifndef TUYA_RF_TEST_H_
#define TUYA_RF_TEST_H_



typedef enum{
	FACTORY_TEST = 0,
	AUTH_RF_TEST,
}scan_rf_type_t;

#define MAX_BUFFER_DATA			(20)
typedef struct _tag_data_stats_t{
	union
	{
	unsigned char length;
	unsigned char index;
	};
	unsigned char data[MAX_BUFFER_DATA];
}data_stats_t;

////////////////

unsigned char tuya_auth_rf_test_begin_stats_flag_get(void);

void tuya_auth_rf_test_begin_stats_flag_set(unsigned char);

char tuya_auth_rf_test_calc_avg(void);

void tuya_auth_rf_test_rssi_stats_add(char rssi);

void tuya_auth_rf_test_rssi_stats_reset(void);

////////////////


void tuya_factory_test_auth_rf_resp(unsigned char suc_flag,char rssi);

scan_rf_type_t tuya_factory_test_scan_type_get(void);

void tuya_start_factory_test_scan(scan_rf_type_t test_type,unsigned int timeout_ms);

/////////////////

//typedef void (*tuya_factory_test_cb_fun)(BOOL success_flag,INT rssi);
//
//void tuya_user_app_regist_factory_test_cb(tuya_factory_test_cb_fun cb);
//
//tuya_factory_test_cb_fun tuya_factory_test_get_cb(void);

void tuya_factorytest_on_scanrsp(unsigned char *dev_name, unsigned char dev_name_len, char dev_rssi);

#endif /* TUYA_RF_TEST_H_ */
