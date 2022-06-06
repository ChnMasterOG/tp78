################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LiteOS/utils/los_debug.c \
../LiteOS/utils/los_error.c \
../LiteOS/utils/los_hook.c 

OBJS += \
./LiteOS/utils/los_debug.o \
./LiteOS/utils/los_error.o \
./LiteOS/utils/los_hook.o 

C_DEPS += \
./LiteOS/utils/los_debug.d \
./LiteOS/utils/los_error.d \
./LiteOS/utils/los_hook.d 


# Each subdirectory must supply rules for building sources it contributes
LiteOS/utils/%.o: ../LiteOS/utils/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused  -g -I"../Debug" -I"../Core" -I"../User" -I"../Peripheral/inc" -I"../LiteOS/components/backtrace" -I"../LiteOS/components/cpup" -I"../LiteOS/components/power" -I"../LiteOS/third_party/bounds_checking_function/include" -I"../LiteOS/third_party/bounds_checking_function/src" -I"../LiteOS/utils/internal" -I"../LiteOS/utils" -I"../LiteOS/kernel/arch/include" -I"../LiteOS/kernel/arch/risc-v/V4A/gcc" -I"../LiteOS/kernel/include" -I"../LiteOS/kernel/src" -I"../LiteOS/kernel/src/mm" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

