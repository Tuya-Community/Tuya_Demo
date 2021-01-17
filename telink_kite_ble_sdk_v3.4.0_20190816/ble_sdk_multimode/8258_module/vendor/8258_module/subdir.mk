################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/8258_module/app.c \
../vendor/8258_module/app_att.c \
../vendor/8258_module/battery_check.c \
../vendor/8258_module/main.c \
../vendor/8258_module/spp.c 

OBJS += \
./vendor/8258_module/app.o \
./vendor/8258_module/app_att.o \
./vendor/8258_module/battery_check.o \
./vendor/8258_module/main.o \
./vendor/8258_module/spp.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/8258_module/%.o: ../vendor/8258_module/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\extern_components\mbedtls" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\include" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\easylogger\inc" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tuya_ble_sdk_port" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tlsr" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_app" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\product_test" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\uart_common" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\vendor\8258_module" -D__PROJECT_8258_MODULE__=1 -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


