################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tuya_ble_sdk/app/SHT3x/i2c.c \
../tuya_ble_sdk/app/SHT3x/sht3x.c \
../tuya_ble_sdk/app/SHT3x/system.c 

OBJS += \
./tuya_ble_sdk/app/SHT3x/i2c.o \
./tuya_ble_sdk/app/SHT3x/sht3x.o \
./tuya_ble_sdk/app/SHT3x/system.o 


# Each subdirectory must supply rules for building sources it contributes
tuya_ble_sdk/app/SHT3x/%.o: ../tuya_ble_sdk/app/SHT3x/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\extern_components\mbedtls" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\sdk\include" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\sdk\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\easylogger\inc" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\tuya_ble_sdk_port" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components\tlsr" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_components" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_app" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\app\product_test" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\app\uart_common" -I"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\vendor\8258_module" -D__PROJECT_8258_MODULE__=1 -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


