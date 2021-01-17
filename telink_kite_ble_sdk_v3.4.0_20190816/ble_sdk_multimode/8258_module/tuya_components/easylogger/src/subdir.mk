################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tuya_components/easylogger/src/elog.c \
../tuya_components/easylogger/src/elog_async.c \
../tuya_components/easylogger/src/elog_buf.c \
../tuya_components/easylogger/src/elog_utils.c 

OBJS += \
./tuya_components/easylogger/src/elog.o \
./tuya_components/easylogger/src/elog_async.o \
./tuya_components/easylogger/src/elog_buf.o \
./tuya_components/easylogger/src/elog_utils.o 


# Each subdirectory must supply rules for building sources it contributes
tuya_components/easylogger/src/%.o: ../tuya_components/easylogger/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\extern_components\mbedtls" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\include" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\easylogger\inc" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tuya_ble_sdk_port" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tlsr" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_app" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\product_test" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\uart_common" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\vendor\8258_module" -D__PROJECT_8258_MODULE__=1 -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


