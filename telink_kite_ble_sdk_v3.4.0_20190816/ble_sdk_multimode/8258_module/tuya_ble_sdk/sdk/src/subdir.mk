################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tuya_ble_sdk/sdk/src/tuya_ble_api.c \
../tuya_ble_sdk/sdk/src/tuya_ble_data_handler.c \
../tuya_ble_sdk/sdk/src/tuya_ble_event.c \
../tuya_ble_sdk/sdk/src/tuya_ble_event_handler.c \
../tuya_ble_sdk/sdk/src/tuya_ble_event_handler_weak.c \
../tuya_ble_sdk/sdk/src/tuya_ble_gatt_send_queue.c \
../tuya_ble_sdk/sdk/src/tuya_ble_heap.c \
../tuya_ble_sdk/sdk/src/tuya_ble_main.c \
../tuya_ble_sdk/sdk/src/tuya_ble_mem.c \
../tuya_ble_sdk/sdk/src/tuya_ble_mutli_tsf_protocol.c \
../tuya_ble_sdk/sdk/src/tuya_ble_queue.c \
../tuya_ble_sdk/sdk/src/tuya_ble_storage.c \
../tuya_ble_sdk/sdk/src/tuya_ble_unix_time.c \
../tuya_ble_sdk/sdk/src/tuya_ble_utils.c 

OBJS += \
./tuya_ble_sdk/sdk/src/tuya_ble_api.o \
./tuya_ble_sdk/sdk/src/tuya_ble_data_handler.o \
./tuya_ble_sdk/sdk/src/tuya_ble_event.o \
./tuya_ble_sdk/sdk/src/tuya_ble_event_handler.o \
./tuya_ble_sdk/sdk/src/tuya_ble_event_handler_weak.o \
./tuya_ble_sdk/sdk/src/tuya_ble_gatt_send_queue.o \
./tuya_ble_sdk/sdk/src/tuya_ble_heap.o \
./tuya_ble_sdk/sdk/src/tuya_ble_main.o \
./tuya_ble_sdk/sdk/src/tuya_ble_mem.o \
./tuya_ble_sdk/sdk/src/tuya_ble_mutli_tsf_protocol.o \
./tuya_ble_sdk/sdk/src/tuya_ble_queue.o \
./tuya_ble_sdk/sdk/src/tuya_ble_storage.o \
./tuya_ble_sdk/sdk/src/tuya_ble_unix_time.o \
./tuya_ble_sdk/sdk/src/tuya_ble_utils.o 


# Each subdirectory must supply rules for building sources it contributes
tuya_ble_sdk/sdk/src/%.o: ../tuya_ble_sdk/sdk/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\extern_components\mbedtls" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\include" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\easylogger\inc" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tuya_ble_sdk_port" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tlsr" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_app" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\product_test" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\uart_common" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\vendor\8258_module" -D__PROJECT_8258_MODULE__=1 -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


