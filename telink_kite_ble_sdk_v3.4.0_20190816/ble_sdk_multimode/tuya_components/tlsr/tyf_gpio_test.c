/*************************************************************************
	> File Name: drv_gpio_base_test.c
	> Author:
	> Mail:
	> Created Time: Tue 26 Mar 2019 15:11:08 CST
 ************************************************************************/
#include "tyf_gpio_test.h"
#include "tuya_log.h"
//-------------------------------------------
//TEST BT3L-825x
//-------------------------------------------
#define    FACTORY_GPIO_TEST_CASE0_PIN_NUM     9
#define    FACTORY_GPIO_TEST_CASE0_PIN_ARRAY   {GPIO_PC2,GPIO_PC3,GPIO_PD2,GPIO_PD7,GPIO_PB4,GPIO_PB5,GPIO_PC4,GPIO_PA0,GPIO_PC0}
#define    FACTORY_GPIO_TEST_CASE0_PIN_MAP     {2,2,1,1,3,3,1,4,4}
//#define    FACTORY_GPIO_TEST_TOTAL_CASES       1
//-------------------------------------------
//TEST BT7L-825x
//-------------------------------------------
#define    FACTORY_GPIO_TEST_CASE1_PIN_NUM     6
#define    FACTORY_GPIO_TEST_CASE1_PIN_ARRAY   {GPIO_PB4,GPIO_PD2,GPIO_PC1,GPIO_PC2,GPIO_PC0,GPIO_PB5,GPIO_PC3}
#define    FACTORY_GPIO_TEST_CASE1_PIN_MAP     {1,1,1,2,2,3,3}

#define    FACTORY_GPIO_TEST_TOTAL_CASES       2


//-------------------------------------------
//TEST TYBT4L-8269
//-------------------------------------------
                                              //R       //CLK    //B       //ww     //SDA   //adc-none   //cw     //G
//#define    FACTORY_GPIO_TEST_CASE0_PIN_ARRAY {GPIO_PB6,GPIO_PA4,GPIO_PE1, GPIO_PB1,GPIO_PA3,             GPIO_PB4,GPIO_PE0}
//#define    FACTORY_GPIO_TEST_CASE0_PIN_MAP   {1,1,1,2,2,3,3}
//#define    FACTORY_GPIO_TEST_CASE0_PIN_NUM   7
//#define    FACTORY_GPIO_TEST_TOTAL_CASES     1


//BASE APP CONFIG/////////////////////////////////////////////////////
//#define FACTORY_GPIO_TEST_TOTAL_CASES 1
//#define FACTORY_GPIO_TEST_CASE1_PIN_NUM 4
//#define FACTORY_GPIO_TEST_CASE1_PIN_ARRAY {0,1,2,4}
//#define FACTORY_GPIO_TEST_CASE1_PIN_MAP {1,1,1,1}
#if FACTORY_GPIO_TEST_TOTAL_CASES != 0
drv_gpio_base_test_s gpio_test_cases[FACTORY_GPIO_TEST_TOTAL_CASES] =
{
    #if FACTORY_GPIO_TEST_TOTAL_CASES >= 1
    {
        .pin_num = FACTORY_GPIO_TEST_CASE0_PIN_NUM,
        .pin = FACTORY_GPIO_TEST_CASE0_PIN_ARRAY,
        .map = FACTORY_GPIO_TEST_CASE0_PIN_MAP,
    },
    #endif
    #if FACTORY_GPIO_TEST_TOTAL_CASES >= 2
    {
        .pin_num = FACTORY_GPIO_TEST_CASE1_PIN_NUM,
        .pin = FACTORY_GPIO_TEST_CASE1_PIN_ARRAY,
        .map = FACTORY_GPIO_TEST_CASE1_PIN_MAP,
    },
    #endif
    #if FACTORY_GPIO_TEST_TOTAL_CASES >= 3
    {
        .pin_num = FACTORY_GPIO_TEST_CASE2_PIN_NUM,
        .pin = FACTORY_GPIO_TEST_CASE2_PIN_ARRAY,
        .map = FACTORY_GPIO_TEST_CASE2_PIN_MAP,
    }
    #endif
};
#endif
//////////////////////////////////////////////////////////////////////



static inline unsigned char check_num(unsigned char *ret,unsigned char num){
	unsigned char i = 0;
    for(;i < ret[0];i++){
        if(ret[i+1] == num){
            return 1;
        }
    }
    return 0;
}

/*
 * 条件：按照map表中的设置将对应引脚互联
 * 算法：所有待测引脚默认拉高输入，对其中任意一个引脚设置为输出低电平，检测其他引脚逻辑是否正确（互联/没有互联）
 * 返回：ret[0]表示异常的引脚数量，ret[1..]表示异常的引脚编号（标号为acill表示）
 *
 * 特别的：对于总IO为1的测试案例，直接外部串联电阻挂在VCC上，这里将该引脚设置为下拉输入，然后读电平是否为高
 */
static inline void drv_gpio_base_test_start(drv_gpio_base_test_s *p_gpio_test)
{
	unsigned int temp = 0,j = 0,i = 0,index = 1;

	unsigned char pin_num = p_gpio_test->pin_num;
    // unsigned int *pin; pin= &( p_gpio_test->pin[0]);
    //unsigned char *map = p_gpio_test->map;
    unsigned char *ret = p_gpio_test->ret;

    ret[0] = 0;
    //tuya_log_d("pin23=%x-%x-%x-%x-%x-%x-%x",p_gpio_test->pin,pin,p_gpio_test->pin[0],pin[0],GPIO_PB6,p_gpio_test->pin[3],gpio_test_cases[0].pin[3],GPIO_PB1);

    if(pin_num == 1)
    {//special gpio_num == 1
    	gpio_set_output_en(p_gpio_test->pin[0],0);
    	gpio_set_input_en(p_gpio_test->pin[0],1);
        gpio_setup_up_down_resistor(p_gpio_test->pin[0],PM_PIN_PULLUP_10K);
        temp = gpio_read(p_gpio_test->pin[0]);
        if(0 == temp){
            ret[0]++;
            ret[index++] = 0 + 0x30;
        }
    }
    else
    {
        for(i=0;i<pin_num;i++)
        {
        	gpio_set_func(p_gpio_test->pin[i],AS_GPIO);
        }

        for(j=0;j<pin_num;j++)
        {
            for(i=0;i<pin_num;i++)
            {
            	if(i!=j)
            	{
					gpio_set_input_en(p_gpio_test->pin[i],1);
					gpio_set_output_en(p_gpio_test->pin[i],0);
					gpio_setup_up_down_resistor(p_gpio_test->pin[i],PM_PIN_PULLUP_10K);
            	}
            }


            gpio_set_input_en(p_gpio_test->pin[j],0);
            gpio_set_output_en(p_gpio_test->pin[j],1);
            gpio_setup_up_down_resistor(p_gpio_test->pin[j], PM_PIN_UP_DOWN_FLOAT);

            gpio_write(p_gpio_test->pin[j],0);

            for(i=0;i<pin_num;i++)
            {
                if(j!=i)
                {
                	temp=gpio_read(p_gpio_test->pin[i]);
                    if(p_gpio_test->map[i] == p_gpio_test->map[j])
                    {
                        if((0 != temp)&&(0 == check_num(ret,i+0x30)))
                        {
                            ret[0]++;
                            ret[index++] = i + 0x30;
                            tuya_log_d("pin1=%d-%d-%d-0x%x-0x%x-0x%x-0x%x-0x%x-0x%x",j,i,temp,p_gpio_test->pin[j],GPIO_PD2,p_gpio_test->pin[i],GPIO_PC4,gpio_read(GPIO_PD2),gpio_read(GPIO_PC4));
                        }
                        else
                        {
                        //	tuya_log_d("pin2=%d-%d-%d-%x-%x-%x",j,i,temp,gpio_test_cases[0].pin[3],GPIO_PB1);
                        }
                    }
                    else
                    {
                        if((0 == temp)&&(0 == check_num(ret,i+0x30)))
                        {
                            ret[0]++;
                            ret[index++] = i + 0x30;
                            tuya_log_d("pin3=%d-%d-%d-%x-%x",j,i,temp,GPIO_PB1);
                        }
                        else
                        {
                        	//tuya_log_d("pin4=%d-%d-%d-%x",j,i,temp);
                        }
                    }
                }
            }
        }
    }
    tuya_log_d("pin test=%d", ret[0]);
}

/*
 * 根据用户配置自动测试，测试成功返回1，失败返回0
 */
unsigned char drv_gpio_base_test_auto(void)
{
#if FACTORY_GPIO_TEST_TOTAL_CASES == 0
    return 1;
#else
    for(unsigned char i=0;i<FACTORY_GPIO_TEST_TOTAL_CASES;i++)
    {
        drv_gpio_base_test_start(&gpio_test_cases[i]);
        if(gpio_test_cases[i].ret[0] == 0)return 1;
    }
    return 0;
#endif
}
