################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include vendor/common/subdir.mk
-include vendor/8258_module/subdir.mk
-include tuya_components/tuya_ble_sdk_port/subdir.mk
-include tuya_components/tlsr/subdir.mk
-include tuya_components/lib/subdir.mk
-include tuya_components/easylogger/src/subdir.mk
-include tuya_components/easylogger/port/subdir.mk
-include tuya_ble_sdk/sdk/src/subdir.mk
-include tuya_ble_sdk/port/subdir.mk
-include tuya_ble_sdk/extern_components/mbedtls/subdir.mk
-include tuya_ble_sdk/app/uart_common/subdir.mk
-include tuya_ble_sdk/app/SHT3x/subdir.mk
-include tuya_ble_app/subdir.mk
-include drivers/8258/subdir.mk
-include common/subdir.mk
-include boot/8258/subdir.mk
-include boot/8253/subdir.mk
-include boot/8251/subdir.mk
-include application/usbstd/subdir.mk
-include application/print/subdir.mk
-include application/keyboard/subdir.mk
-include application/app/subdir.mk
-include objects.mk

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LST += \
8258_module.lst \

FLASH_IMAGE += \

SIZEDUMMY += \
sizedummy \


# All Target
all: ble_sdk_multimode.elf secondary-outputs

# Tool invocations
ble_sdk_multimode.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: TC32 C Linker'
	tc32-elf-ld --gc-sections -L"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\proj_lib" -L"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode\tuya_ble_sdk\sdk\lib" -T ../boot.link -o"ble_sdk_multimode.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

8258_module.lst: ble_sdk_multimode.elf
	@echo 'Invoking: TC32 Create Extended Listing'
	tc32-elf-objdump -x -D -l -S ble_sdk_multimode.elf  >"8258_module.lst"
	@echo 'Finished building: $@'
	@echo ' '

: ble_sdk_multimode.elf
	@echo 'Create Flash image (binary format)'
	tc32-elf-objcopy -O binary ble_sdk_multimode.elf
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: ble_sdk_multimode.elf
	@echo 'Invoking: Print Size'
	tc32-elf-size -t ble_sdk_multimode.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(LST)$(SIZEDUMMY) ble_sdk_multimode.elf
	-@echo ' '

post-build:
	-"D:\Users\Desktop\BLE_SOC_DEMO\tuya_ble_sdk_Demo_Project_tlsr8253-master\telink_kite_ble_sdk_v3.4.0_20190816\ble_sdk_multimode/tl_check_fw.sh"   8258_module   ble_sdk_multimode
	-@echo ' '

secondary-outputs: $(LST) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
