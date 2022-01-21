################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/main.c 

OBJS += \
./APP/main.o 

C_DEPS += \
./APP/main.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Startup" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\HAL\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Profile\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Ld" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\LIB" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

