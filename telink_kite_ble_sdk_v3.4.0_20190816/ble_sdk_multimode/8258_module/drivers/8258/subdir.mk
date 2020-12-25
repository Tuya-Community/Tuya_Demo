################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/8258/adc.c \
../drivers/8258/aes.c \
../drivers/8258/analog.c \
../drivers/8258/audio.c \
../drivers/8258/bsp.c \
../drivers/8258/clock.c \
../drivers/8258/emi.c \
../drivers/8258/flash.c \
../drivers/8258/gpio_8258.c \
../drivers/8258/i2c.c \
../drivers/8258/lpc.c \
../drivers/8258/qdec.c \
../drivers/8258/rf_pa.c \
../drivers/8258/s7816.c \
../drivers/8258/spi.c \
../drivers/8258/timer.c \
../drivers/8258/uart.c \
../drivers/8258/watchdog.c 

OBJS += \
./drivers/8258/adc.o \
./drivers/8258/aes.o \
./drivers/8258/analog.o \
./drivers/8258/audio.o \
./drivers/8258/bsp.o \
./drivers/8258/clock.o \
./drivers/8258/emi.o \
./drivers/8258/flash.o \
./drivers/8258/gpio_8258.o \
./drivers/8258/i2c.o \
./drivers/8258/lpc.o \
./drivers/8258/qdec.o \
./drivers/8258/rf_pa.o \
./drivers/8258/s7816.o \
./drivers/8258/spi.o \
./drivers/8258/timer.o \
./drivers/8258/uart.o \
./drivers/8258/watchdog.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/8258/%.o: ../drivers/8258/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\extern_components\mbedtls" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\sdk\include" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\sdk\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\easylogger\inc" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\tuya_ble_sdk_port" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\tlsr" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_app" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\app\product_test" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\app\uart_common" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\vendor\8258_module" -D__PROJECT_8258_MODULE__=1 -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


