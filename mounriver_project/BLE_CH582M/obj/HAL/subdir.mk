################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/BATTERY.c \
../HAL/BLE.c \
../HAL/KEY.c \
../HAL/KEYBOARD.c \
../HAL/LED.c \
../HAL/MCU.c \
../HAL/OLED.c \
../HAL/OLEDFONT.c \
../HAL/PS2.c \
../HAL/RTC.c \
../HAL/SLEEP.c \
../HAL/USB.c \
../HAL/WS2812.c 

OBJS += \
./HAL/BATTERY.o \
./HAL/BLE.o \
./HAL/KEY.o \
./HAL/KEYBOARD.o \
./HAL/LED.o \
./HAL/MCU.o \
./HAL/OLED.o \
./HAL/OLEDFONT.o \
./HAL/PS2.o \
./HAL/RTC.o \
./HAL/SLEEP.o \
./HAL/USB.o \
./HAL/WS2812.o 

C_DEPS += \
./HAL/BATTERY.d \
./HAL/BLE.d \
./HAL/KEY.d \
./HAL/KEYBOARD.d \
./HAL/LED.d \
./HAL/MCU.d \
./HAL/OLED.d \
./HAL/OLEDFONT.d \
./HAL/PS2.d \
./HAL/RTC.d \
./HAL/SLEEP.d \
./HAL/USB.d \
./HAL/WS2812.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\tp78\mounriver_project\BLE_CH582M\Startup" -I"E:\tp78\mounriver_project\BLE_CH582M\HAL\include" -I"E:\tp78\mounriver_project\BLE_CH582M\Profile\include" -I"E:\tp78\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"E:\tp78\mounriver_project\BLE_CH582M\Ld" -I"E:\tp78\mounriver_project\BLE_CH582M\LIB" -I"E:\tp78\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

