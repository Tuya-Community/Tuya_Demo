/*************************************************************************
	> File Name: drv_gpio_base_test.h
	> Author:
	> Mail:
	> Created Time: Tue 26 Mar 2019 15:11:13 CST
 ************************************************************************/

#ifndef _DRV_GPIO_BASE_TEST_H
#define _DRV_GPIO_BASE_TEST_H

#include "../../drivers/8258/gpio_8258.h"

#define MAX_GPIO_TEST_PIN           9
#define MAX_GPIO_TEST_PIN_MORE      (MAX_GPIO_TEST_PIN+1)   //=8+1

typedef struct{
    unsigned char pin_num;
    unsigned int pin[MAX_GPIO_TEST_PIN];
    unsigned char map[MAX_GPIO_TEST_PIN];
    unsigned char ret[MAX_GPIO_TEST_PIN_MORE];
}drv_gpio_base_test_s;


extern unsigned char drv_gpio_base_test_auto(void);


#endif
