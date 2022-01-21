################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Profile/battservice.c \
../Profile/devinfoservice.c \
../Profile/hiddev.c \
../Profile/hidkbmservice.c \
../Profile/scanparamservice.c 

OBJS += \
./Profile/battservice.o \
./Profile/devinfoservice.o \
./Profile/hiddev.o \
./Profile/hidkbmservice.o \
./Profile/scanparamservice.o 

C_DEPS += \
./Profile/battservice.d \
./Profile/devinfoservice.d \
./Profile/hiddev.d \
./Profile/hidkbmservice.d \
./Profile/scanparamservice.d 


# Each subdirectory must supply rules for building sources it contributes
Profile/%.o: ../Profile/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Startup" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\HAL\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Profile\include" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\Ld" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\LIB" -I"Y:\tp78-main\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

