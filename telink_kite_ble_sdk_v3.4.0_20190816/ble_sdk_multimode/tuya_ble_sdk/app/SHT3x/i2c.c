#include "i2c.h"
#include "gpio_8258.h"
//#include "hal_gpio.h"

#define PIN_SCL         GPIO_PB4
#define PIN_SDA         GPIO_PD2


void scl_reset(void){
    gpio_write(PIN_SCL,0);
}

void scl_set(void){
    gpio_write(PIN_SCL,1);
}

void sda_reset(void){
    gpio_write(PIN_SDA,0);
}

void sda_set(void){
    gpio_write(PIN_SDA,1);
}

void scl_out(void){
	gpio_set_input_en(PIN_SCL,0);
	gpio_set_output_en(PIN_SCL,1);
}
void scl_in(void){
	gpio_set_input_en(PIN_SCL,1);
	gpio_set_output_en(PIN_SCL,0);

}
void sda_out(void){
	gpio_set_input_en(PIN_SDA,0);
	gpio_set_output_en(PIN_SDA,1);

}
void sda_in(void){
	gpio_set_input_en(PIN_SDA,1);
	gpio_set_output_en(PIN_SDA,0);

}


u8 sda_read(void){
    u8 state;
    sda_in();

    state = (gpio_read(PIN_SDA) == 0 ? 0:1);
    
    return state;
}

/*******************************************************************************
* @fn        i2c_init
*
* @brief    init
*
* @return  none
*
******************************************************************************/
void i2c_init(void){
    gpio_set_func(PIN_SDA,AS_GPIO);
    gpio_set_func(PIN_SCL,AS_GPIO);
    sda_out();
    scl_out();

    gpio_setup_up_down_resistor(PIN_SDA, PM_PIN_PULLUP_10K);
    gpio_setup_up_down_resistor(PIN_SCL, PM_PIN_PULLUP_10K);

}

/*******************************************************************************
* @fn        i2c_delay
*
* @brief    i2c  delay 5us
*
* @return  none
*
******************************************************************************/
void i2c_delay(void)
{
    volatile uint_8 j = 10;
    
    while(j)
    {
        j --;
    }
}

/*******************************************************************************
* @fn        i2c_noack
*
* @brief    i2c本芯片不应答
*
* @return  none
*
******************************************************************************/
void i2c_noack(void){
    scl_reset();
    i2c_delay();
    sda_set();
    i2c_delay();
    scl_set();
    i2c_delay();
    scl_reset();
    i2c_delay();
}

/*******************************************************************************
* @fn        i2c_ack
*
* @brief    i2c本芯片应答
* *
* @return  none
*
******************************************************************************/
void i2c_ack(void){
    sda_out();
    
//    scl_reset();
//    i2c_delay();
    sda_reset();
    // i2c_delay();
    scl_set();
    i2c_delay();
    scl_reset();
    i2c_delay();
}

/*******************************************************************************
* @fn        i2c_GPIO_stops
*
* @brief    i2c stop
*
* @return  none
*
******************************************************************************/
void i2c_stop(void){
    sda_out();
    sda_reset();

    scl_set();
    i2c_delay();
    sda_set();
    i2c_delay();

}

/*******************************************************************************
* @fn        i2c_start
*
* @brief    i2c begin
*
* @return  none
*
******************************************************************************/
u8 i2c_start(void){
    sda_out();
    sda_set();

    scl_set();
    i2c_delay();
    sda_reset();
    i2c_delay();

}
/*******************************************************************************
* @fn        i2c_wait_ack
*
* @brief    i2c wait ack
*
* @return  none
*
******************************************************************************/
u8 i2c_wait_ack(void){
    u8 cnt = 200;
    
    //////////////////////////此处为金句，IO输入输出切换的瞬间有抖动，造成异常应答
    sda_read();
    //////////////////////////
    scl_set();
    i2c_delay();
    
    while( sda_read() )
    {
      cnt --;
      // i2c_delay();
    
      if(cnt==0)
      {
         scl_reset();
          return 1;
      }
      i2c_delay();
    }
    i2c_delay();
    scl_reset();
    i2c_delay();
    return 0;

}

/*******************************************************************************
* @fn        i2c_send_byte
*
* @brief    i2c send a byte
*
* @return  none
*
******************************************************************************/
void i2c_send_byte( u8 byte ){
    u8 i = 8;
    sda_out();
    while (i--) {
        scl_reset();
        if (byte & 0x80)
            sda_set();
        else
            sda_reset();
        byte <<= 1;
        i2c_delay();
        scl_set();
        i2c_delay();
    }
    scl_reset();
    i2c_delay();

}

/*******************************************************************************
* @fn        i2c_read_byte
*
* @brief    i2c read a byte

* @return  none
*
******************************************************************************/
u8 i2c_read_byte(void){
      u8 i = 8;
      u8 byte = 0;
    
      sda_set();
      sda_read();
    while (i--) 
    {
        byte <<= 1;
        scl_reset();
        i2c_delay();
        scl_set();
        i2c_delay();
        if (sda_read())
        {
              byte |= 0x01;
        }
    }
    scl_reset();
    i2c_delay();
    
    return byte;

}

