################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tuya_components/tlsr/nv.c \
../tuya_components/tlsr/tuya_factory_test.c \
../tuya_components/tlsr/tuya_flash.c \
../tuya_components/tlsr/tuya_log.c \
../tuya_components/tlsr/tuya_ota.c \
../tuya_components/tlsr/tuya_rf_test.c \
../tuya_components/tlsr/tuya_utils.c \
../tuya_components/tlsr/ty_broad.c \
../tuya_components/tlsr/tyf_gpio_test.c 

OBJS += \
./tuya_components/tlsr/nv.o \
./tuya_components/tlsr/tuya_factory_test.o \
./tuya_components/tlsr/tuya_flash.o \
./tuya_components/tlsr/tuya_log.o \
./tuya_components/tlsr/tuya_ota.o \
./tuya_components/tlsr/tuya_rf_test.o \
./tuya_components/tlsr/tuya_utils.o \
./tuya_components/tlsr/ty_broad.o \
./tuya_components/tlsr/tyf_gpio_test.o 


# Each subdirectory must supply rules for building sources it contributes
tuya_components/tlsr/%.o: ../tuya_components/tlsr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\extern_components\mbedtls" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\include" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\sdk\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\easylogger\inc" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\lib" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tuya_ble_sdk_port" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components\tlsr" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_components" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_app" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\product_test" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\tuya_ble_sdk\app\uart_common" -I"D:\Users\Desktop\BLE_SOC_DEMO\ble_sdk_multimode\vendor\8258_module" -D__PROJECT_8258_MODULE__=1 -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


