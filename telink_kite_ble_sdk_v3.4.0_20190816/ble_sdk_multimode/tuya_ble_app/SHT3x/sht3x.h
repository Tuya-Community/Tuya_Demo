#pragma once
#include "../common/types.h"
//#include <string.h>
// SHT30 Temperature and humidity macros
#define I2C_WRITE                       0
#define I2C_READ                        1

#define SHT30_ADDR                      (0x44<<1)     /* I2C write hearder */
#define SHT30_RESET_MSG                 {0x30,0xa2}   /* SHT30 soft reset cmd */
#define SHT30_RESET_MSG_LEN             2
#define SHT30_READ_CFG_MSG              {0x2c,0x06}   /* read data config cmd */
#define SHT30_READ_CFG_MSG_LEN          2
#define SHT30_REG_STATUS_GET            {0xf3,0x2d}   /* status register get cmd */
#define SHT30_REG_STATUS_GET_LEN        2

#define SHT30_REG_STATUS_CLEAR          {0x30,0x41}   /* status register clear cmd */
#define SHT30_REG_STATUS_CLEAR_LEN      2

#define SHT30_RESPONSE_LEN              6
#define SHT30_REG_STATUS_LEN            3

#define SHT30_CONVERSION_TIME_MS        13




void tem_hum_i2c_io_init();
u8 sensor_sht30_read_data_finsh_callback(u8 *buff,short* temp_ten,u8* hum_ten);
u8 tem_hum_i2c_start_read(short* temp_ten,u8* hum_ten);
