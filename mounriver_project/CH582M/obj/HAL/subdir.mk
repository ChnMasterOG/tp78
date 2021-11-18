################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/PS2Driver.c \
../HAL/UART1.c \
../HAL/USBDriver.c 

OBJS += \
./HAL/PS2Driver.o \
./HAL/UART1.o \
./HAL/USBDriver.o 

C_DEPS += \
./HAL/PS2Driver.d \
./HAL/UART1.d \
./HAL/USBDriver.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DDEBUG=1 -I"../StdPeriphDriver/inc" -I"../RVMSIS" -I"../HAL/include" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

