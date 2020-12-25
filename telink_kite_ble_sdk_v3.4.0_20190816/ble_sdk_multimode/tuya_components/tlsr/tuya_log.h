/*
 * tuya_log.h
 *
 *  Created on: 2017-10-24
 *      Author: echo
 */

#ifndef TUYA_LOG_H_
#define TUYA_LOG_H_

//#include "tuya_ble_type.h"
#include <stdarg.h>
#include "elog.h"


void tuya_log_init();

#define	tuya_log(...)   log_v(__VA_ARGS__)
#define	tuya_log_a(...) log_a(__VA_ARGS__)
#define	tuya_log_e(...) log_e(__VA_ARGS__)
#define	tuya_log_w(...) log_w(__VA_ARGS__)
#define	tuya_log_i(...) log_i(__VA_ARGS__)
#define	tuya_log_d(...) log_d(__VA_ARGS__)
#define	tuya_log_v(...) log_v(__VA_ARGS__)

void tuya_log_dumpHex(const char *name, unsigned char width, unsigned char *buf, unsigned short size);

#endif /* TUYA_LOG_H_ */
