#include "sht3x.h"
#include "../../drivers/8258/timer.h"
#include "../../drivers/8258/i2c.h"
#include "tuya_ble_log.h"
#define HARD_I2C_ENABLE 0

#define I2C_SCL         GPIO_PB4
#define I2C_SDA         GPIO_PD2

int SHT30_temperature;
u8 SHT30_humidity;

enum{
	gpio_input_mode,
	gpio_output_mode,
};

static void i2c_delay(unsigned long tim_1us)
{
	sleep_us(tim_1us);
}

static void i2c_sda_pin_mode_set(u8 mode,u8 level)
{
	if(mode == gpio_input_mode){
		gpio_set_input_en(I2C_SDA,1);
		gpio_set_output_en(I2C_SDA,0);
	}else if(mode == gpio_output_mode){
		gpio_set_input_en(I2C_SDA,0);
		gpio_set_output_en(I2C_SDA,1);
	}
}
static void i2c_sda_pin_set(u8 level)
{
	gpio_write(I2C_SDA, level);
}
static void i2c_scl_pin_set(u8 level)
{
	gpio_write(I2C_SCL, level);

}
static u8 i2c_sda_pin_status_get(void)
{
    return gpio_read(I2C_SDA)?1:0;
}

/**
 * @description: i2c ack func
 * @param {type} none
 * @return: none
 */
static void i2c_ack(void)
{
    i2c_scl_pin_set(0);
    i2c_delay(25);

    i2c_sda_pin_mode_set( gpio_output_mode, 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(25);

    i2c_scl_pin_set( 1 );
    i2c_delay(25);
    i2c_scl_pin_set( 0 );
    i2c_delay(25);
}

/**
 * @description: i2c none ack func
 * @param {type} none
 * @return: none
 */
static void i2c_noack(void)
{
    i2c_sda_pin_mode_set( gpio_output_mode, 1 );
    i2c_sda_pin_set( 1 );

    i2c_delay(25);
    i2c_scl_pin_set( 1 );
    i2c_delay(25);
    i2c_scl_pin_set( 0 );
    i2c_delay(25);
}

/**
 * @description: i2c wait ack
 * @param {type} none
 * @return: rev ack return true else return false
 */
static u8 i2c_wait_ack(void)
{
    //u8 state = 0;
    u8 cnt = 50;

    i2c_sda_pin_mode_set( gpio_input_mode, 1 );/* set input and release SDA */
    i2c_sda_pin_set( 1 );
    i2c_delay(25);

    i2c_scl_pin_set( 0 );       /* put down SCL ready to cheack SCA status */
    i2c_delay(25);

    i2c_scl_pin_set( 1 );
    i2c_delay(25);
    while( i2c_sda_pin_status_get() )       /* get ack */
    {
        cnt--;
        if( cnt==0 )
        {
            i2c_scl_pin_set( 0 );
            return FALSE;
        }
        i2c_delay(25);
    }
    i2c_scl_pin_set( 0 );
    i2c_delay(25);
    return TRUE;
}

/**
 * @description: i2c start signal
 * @param {type} none
 * @return: none
 */
void i2c_start(void)
{
    i2c_sda_pin_mode_set( gpio_output_mode, 1 );    //SDA output mode

    i2c_scl_pin_set( 1 );
    i2c_sda_pin_set( 1 );
    i2c_delay(25);

    i2c_sda_pin_set( 0 );
    i2c_delay(25);

    i2c_scl_pin_set( 0 );
    i2c_delay(25);
}

/**
 * @description: i2c stop signal
 * @param {type} none
 * @return: none
 */
void i2c_stop(void)
{
    i2c_sda_pin_mode_set( gpio_output_mode, 0 );     //SDA input mode

    i2c_scl_pin_set( 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(25);

    i2c_scl_pin_set( 1 );
    i2c_delay(25);

    i2c_sda_pin_set( 1 );
    i2c_delay(25);
}

/**
 * @description: send one byte to i2c bus
 * @param {uint8_t} data send to i2c
 * @return: none
 */
void i2c_send_byte(u8 data)
{
    u8 i = 0;
    i2c_scl_pin_set( 0 );
    i2c_sda_pin_mode_set( gpio_output_mode, 1 );
    for( i=0; i<8; i++ )
    {
        if( data & 0x80 )
        {
            i2c_sda_pin_set( 1 );
        }
        else
        {
            i2c_sda_pin_set( 0 );
        }
        i2c_delay(25);
        i2c_scl_pin_set( 1 );
        i2c_delay(25);
        i2c_scl_pin_set( 0 );
        i2c_delay(25);
        data <<= 1;
    }
}

/**
 * @description: send bytes to i2c bus
 * @param {type} none
 * @return: none
 */
void i2c_send_bytes(u8 adderss_cmd, u8 *buff, u8 len)
{
    u8 i;
    i2c_send_byte( adderss_cmd );
    i2c_wait_ack();

    for( i=0;i<len;i++ )
    {
        i2c_send_byte( buff[i] );
        i2c_wait_ack();
    }
}

/**
 * @description: recive one byte from i2c bus
 * @param {type} none
 * @return: none
 */
void i2c_rcv_byte(u8 *data)
{
    u8 i;
    i2c_sda_pin_mode_set( gpio_input_mode, 1 );
    i2c_delay(25);
    for( i=0;i<8;i++ )
    {
        i2c_scl_pin_set( 0 );
        i2c_delay(25);

        i2c_scl_pin_set( 1 );
        *data = *data << 1;
        if( i2c_sda_pin_status_get() )
        {
            *data |= 1;
        }
        i2c_delay(25);
    }
    i2c_scl_pin_set( 0 );
}

/**
 * @description: recive bytes from i2c bus,last byte none ack
 * @param {type} none
 * @return: none
 */
void i2c_rcv_bytes(u8 adderss_cmd, u8 *buff, u8 len)
{
    u8 i;
    i2c_send_byte( adderss_cmd );
    i2c_wait_ack();
    for( i=0;i<len;i++ )
    {
        i2c_rcv_byte( &buff[i] );
        if( i<len-1 )
            i2c_ack();
        else
            i2c_noack();
    }
}


void tem_hum_i2c_soft_init()
{
	gpio_set_func(I2C_SDA,AS_GPIO);
//	gpio_setup_up_down_resistor(I2C_SDA,PM_PIN_PULLUP_10K);
	gpio_set_output_en(I2C_SDA,1);
	gpio_write(I2C_SDA, 1);

	gpio_set_func(I2C_SCL,AS_GPIO);
//	gpio_setup_up_down_resistor(I2C_SCL,PM_PIN_PULLUP_10K);
	gpio_set_output_en(I2C_SCL,1);
	gpio_write(I2C_SCL, 1);
}

void tem_hum_i2c_start(void)
{
    i2c_sda_pin_mode_set( gpio_output_mode, 1 );    //SDA output mode

    i2c_scl_pin_set( 1 );
    i2c_sda_pin_set( 1 );
    i2c_delay(25);

    i2c_sda_pin_set( 0 );
    i2c_delay(25);

    i2c_scl_pin_set( 0 );
    i2c_delay(25);
}

/**
 * @description: i2c stop signal
 * @param {type} none
 * @return: none
 */
void tem_hum_i2c_stop(void)
{
    i2c_sda_pin_mode_set( gpio_output_mode, 0 );     //SDA input mode

    i2c_scl_pin_set( 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(25);

    i2c_scl_pin_set( 1 );
    i2c_delay(25);

    i2c_sda_pin_set( 1 );
    i2c_delay(25);
}


int tem_hum_i2c_read(u8* buff)
{
#if HARD_I2C_ENABLE
	u8 r = irq_disable();
	//	i2c_read_series(0,0,buff,4);
	i2c_read_series(SHT30_ADDR|I2C_READ,1,buff,6);
	irq_restore(r);
#else
    i2c_start();
    i2c_rcv_bytes( SHT30_ADDR | I2C_READ, buff, SHT30_RESPONSE_LEN );
    i2c_stop();
#endif
}

void tem_hum_i2c_soft_reset()
{
    u8 rst_msg[] = SHT30_RESET_MSG;
#if HARD_I2C_ENABLE
	i2c_write_series(SHT30_ADDR|I2C_WRITE,1,rst_msg,SHT30_RESET_MSG_LEN);
#else
    i2c_start();
    i2c_send_bytes( SHT30_ADDR | I2C_WRITE, rst_msg, SHT30_RESET_MSG_LEN );
    i2c_stop();

#endif

}

void tem_hum_i2c_mode_cfg()
{
    u8 cfg_msg[] = SHT30_READ_CFG_MSG;
#if HARD_I2C_ENABLE
	i2c_write_series(SHT30_ADDR|I2C_WRITE,1,cfg_msg,SHT30_READ_CFG_MSG_LEN);
#else
    i2c_start();
    i2c_send_bytes( SHT30_ADDR | I2C_WRITE, cfg_msg, SHT30_READ_CFG_MSG_LEN );
    i2c_stop();
#endif

}
void tem_hum_i2c_io_init()
{
#if HARD_I2C_ENABLE
    i2c_master_init(SHT30_ADDR,8);//set clk to 500k
    i2c_gpio_set(I2C_GPIO_GROUP_C0C1);
//	tem_hum_i2c_soft_reset();
#else
	tem_hum_i2c_soft_init();
	tem_hum_i2c_soft_reset();

#endif
}

u8 sensor_sht30_read_data_finsh_callback(u8 *buff,short* temp_ten,u8* hum_ten);

int temp_humi_i2c_collect_and_report()
{
	//if(true == tem_hum_i2c_start_read(&sensor_save.temperature,&sensor_save.humidity)){
		//mesh_vendor_dp_report(temperature_dp,sensor_save.temperature);
		//mesh_vendor_dp_report(humidity_dp,sensor_save.humidity);
	//}else{
		//APP_LOG("error: collect tem_hum failed\r\n");
	//}
	return -1;
}

u8 tem_hum_i2c_start_read(short* temp_ten,u8* hum_ten)
{
	static u8 buff[6];
	static u8 err_cnt = 0;
	memset(buff,0,6);
	tem_hum_i2c_mode_cfg();
	sleep_us(13000);

	tem_hum_i2c_read(buff);


	u8 ret_status = sensor_sht30_read_data_finsh_callback(buff,temp_ten,hum_ten);
	if(ret_status == false){
		TUYA_APP_LOG_INFO("collect failed\r\n");
		tem_hum_i2c_soft_reset();
		//if(err_cnt < 20){
		//	err_cnt++;
		//	ty_timer_event_delete(temp_humi_i2c_collect_and_report);
		//	ty_timer_event_add(temp_humi_i2c_collect_and_report,5*1000);
			//TODO: REPEAT
		//}
	}else{
		err_cnt = 0;
	}
	return ret_status;

}
u8 crc_array[256] = {
        0x00,0x31,0x62,0x53,0xc4,0xf5,0xa6,0x97,0xb9,0x88,0xdb,0xea,0x7d,0x4c,0x1f,0x2e,
		    0x43,0x72,0x21,0x10,0x87,0xb6,0xe5,0xd4,0xfa,0xcb,0x98,0xa9,0x3e,0x0f,0x5c,0x6d,
		    0x86,0xb7,0xe4,0xd5,0x42,0x73,0x20,0x11,0x3f,0x0e,0x5d,0x6c,0xfb,0xca,0x99,0xa8,
		    0xc5,0xf4,0xa7,0x96,0x01,0x30,0x63,0x52,0x7c,0x4d,0x1e,0x2f,0xb8,0x89,0xda,0xeb,
		    0x3d,0x0c,0x5f,0x6e,0xf9,0xc8,0x9b,0xaa,0x84,0xb5,0xe6,0xd7,0x40,0x71,0x22,0x13,
		    0x7e,0x4f,0x1c,0x2d,0xba,0x8b,0xd8,0xe9,0xc7,0xf6,0xa5,0x94,0x03,0x32,0x61,0x50,
		    0xbb,0x8a,0xd9,0xe8,0x7f,0x4e,0x1d,0x2c,0x02,0x33,0x60,0x51,0xc6,0xf7,0xa4,0x95,
		    0xf8,0xc9,0x9a,0xab,0x3c,0x0d,0x5e,0x6f,0x41,0x70,0x23,0x12,0x85,0xb4,0xe7,0xd6,
		    0x7a,0x4b,0x18,0x29,0xbe,0x8f,0xdc,0xed,0xc3,0xf2,0xa1,0x90,0x07,0x36,0x65,0x54,
		    0x39,0x08,0x5b,0x6a,0xfd,0xcc,0x9f,0xae,0x80,0xb1,0xe2,0xd3,0x44,0x75,0x26,0x17,
		    0xfc,0xcd,0x9e,0xaf,0x38,0x09,0x5a,0x6b,0x45,0x74,0x27,0x16,0x81,0xb0,0xe3,0xd2,
		    0xbf,0x8e,0xdd,0xec,0x7b,0x4a,0x19,0x28,0x06,0x37,0x64,0x55,0xc2,0xf3,0xa0,0x91,
		    0x47,0x76,0x25,0x14,0x83,0xb2,0xe1,0xd0,0xfe,0xcf,0x9c,0xad,0x3a,0x0b,0x58,0x69,
		    0x04,0x35,0x66,0x57,0xc0,0xf1,0xa2,0x93,0xbd,0x8c,0xdf,0xee,0x79,0x48,0x1b,0x2a,
		    0xc1,0xf0,0xa3,0x92,0x05,0x34,0x67,0x56,0x78,0x49,0x1a,0x2b,0xbc,0x8d,0xde,0xef,
		    0x82,0xb3,0xe0,0xd1,0x46,0x77,0x24,0x15,0x3b,0x0a,0x59,0x68,0xff,0xce,0x9d,0xac
            };

u8 CRC8_Table(u8 *p, u8 counter)
{
    u8 crc8 = 0xFF;
    for( ; counter > 0; counter--)
    {
       crc8 = crc_array[crc8^*p];
       p++;
    }
  return crc8;
}

u8 sensor_sht30_read_data_finsh_callback(u8 *buff,short* temp_ten,u8* hum_ten)
{
    u16 temp, hum;
    if( ( CRC8_Table(&buff[0],2) != buff[2] ) || ( CRC8_Table(&buff[3],2) != buff[5] ) )
    {
        return false;
    }

    temp = ((u16)buff[0]<<8) | buff[1];
    hum = ((u16)buff[3]<<8) | buff[4];

    *hum_ten = ( hum*100 ) / 65535;                           /* 这里放大了10倍*/
	if(*hum_ten > 100) *hum_ten = 100;
	if(*hum_ten <0)     *hum_ten = 0;

    *temp_ten = (temp * 175*10) / 65535 - 450;         /* 这里放大了10倍*/
	if(*temp_ten > 20000) *temp_ten = 20000;
	if(*temp_ten < -4000) *temp_ten = 4000;
    //APP_LOG( "------T:%d H:%d\r\n", *temp_ten, *hum_ten );
    return true;
}

