################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../boot/8251/cstartup_8251.S \
../boot/8251/cstartup_8251_RET_16K.S \
../boot/8251/cstartup_8251_RET_32K.S 

OBJS += \
./boot/8251/cstartup_8251.o \
./boot/8251/cstartup_8251_RET_16K.o \
./boot/8251/cstartup_8251_RET_32K.o 


# Each subdirectory must supply rules for building sources it contributes
boot/8251/%.o: ../boot/8251/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 CC/Assembler'
	tc32-elf-gcc -DMCU_STARTUP_8258_RET_32K -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


