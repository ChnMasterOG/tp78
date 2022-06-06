################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LiteOS/components/backtrace/los_backtrace.c 

OBJS += \
./LiteOS/components/backtrace/los_backtrace.o 

C_DEPS += \
./LiteOS/components/backtrace/los_backtrace.d 


# Each subdirectory must supply rules for building sources it contributes
LiteOS/components/backtrace/%.o: ../LiteOS/components/backtrace/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused  -g -I"../Debug" -I"../Core" -I"../User" -I"../Peripheral/inc" -I"../LiteOS/components/backtrace" -I"../LiteOS/components/cpup" -I"../LiteOS/components/power" -I"../LiteOS/third_party/bounds_checking_function/include" -I"../LiteOS/third_party/bounds_checking_function/src" -I"../LiteOS/utils/internal" -I"../LiteOS/utils" -I"../LiteOS/kernel/arch/include" -I"../LiteOS/kernel/arch/risc-v/V4A/gcc" -I"../LiteOS/kernel/include" -I"../LiteOS/kernel/src" -I"../LiteOS/kernel/src/mm" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

