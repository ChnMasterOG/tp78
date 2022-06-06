################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Startup/startup_ch32v10x.S 

OBJS += \
./Startup/startup_ch32v10x.o 

S_UPPER_DEPS += \
./Startup/startup_ch32v10x.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/%.o: ../Startup/%.S
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused  -g -x assembler-with-cpp -I"../Startup" -I"../LiteOS/components/backtrace" -I"../LiteOS/components/cpup" -I"../LiteOS/components/power" -I"../LiteOS/third_party/bounds_checking_function/include" -I"../LiteOS/third_party/bounds_checking_function/src" -I"../LiteOS/utils/internal" -I"../LiteOS/utils" -I"../LiteOS/kernel/arch/include" -I"../LiteOS/kernel/arch/risc-v/V4A/gcc" -I"../LiteOS/kernel/include" -I"../LiteOS/kernel/src" -I"../LiteOS/kernel/src/mm" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

