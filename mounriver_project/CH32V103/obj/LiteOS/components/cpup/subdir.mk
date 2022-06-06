################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LiteOS/components/cpup/los_cpup.c 

OBJS += \
./LiteOS/components/cpup/los_cpup.o 

C_DEPS += \
./LiteOS/components/cpup/los_cpup.d 


# Each subdirectory must supply rules for building sources it contributes
LiteOS/components/cpup/%.o: ../LiteOS/components/cpup/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused  -g -I"../Debug" -I"../Core" -I"../User" -I"../Peripheral/inc" -I"../LiteOS/components/backtrace" -I"../LiteOS/components/cpup" -I"../LiteOS/components/power" -I"../LiteOS/third_party/bounds_checking_function/include" -I"../LiteOS/third_party/bounds_checking_function/src" -I"../LiteOS/utils/internal" -I"../LiteOS/utils" -I"../LiteOS/kernel/arch/include" -I"../LiteOS/kernel/arch/risc-v/V4A/gcc" -I"../LiteOS/kernel/include" -I"../LiteOS/kernel/src" -I"../LiteOS/kernel/src/mm" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

