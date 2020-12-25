/*************************************************************************
	> File Name: app_i2c.h
	> Author: 
	> Mail: 
	> Created Time: 2018.4.16
 ************************************************************************/
//#include "light_types.h"
#include "tl_common.h"

#ifndef _I2C_H__
#define _I2C_H__

void scl_reset(void);

void scl_set(void);

void sda_reset(void);

void sda_set(void);

void scl_out(void);

void scl_in(void);

void sda_out(void);

void sda_in(void);

u8 sda_read(void);

void i2c_init(void);

void i2c_delay(void);

void i2c_noack(void);

void i2c_ack(void);

void i2c_stop(void);

u8 i2c_start(void);

u8 i2c_wait_ack(void);

void i2c_send_byte(u8 byte);

u8 i2c_read_byte(void);





#endif
