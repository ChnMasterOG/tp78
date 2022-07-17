################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/main.c \
../APP/snake.c 

OBJS += \
./APP/main.o \
./APP/snake.o 

C_DEPS += \
./APP/main.d \
./APP/snake.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\tp78\mounriver_project\BLE_CH582M\Startup" -I"E:\tp78\mounriver_project\BLE_CH582M\HAL\include" -I"E:\tp78\mounriver_project\BLE_CH582M\APP\include" -I"E:\tp78\mounriver_project\BLE_CH582M\Profile\include" -I"E:\tp78\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"E:\tp78\mounriver_project\BLE_CH582M\Ld" -I"E:\tp78\mounriver_project\BLE_CH582M\LIB" -I"E:\tp78\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

