################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LiteOS/kernel/arch/risc-v/V4A/gcc/los_context.c \
../LiteOS/kernel/arch/risc-v/V4A/gcc/los_interrupt.c \
../LiteOS/kernel/arch/risc-v/V4A/gcc/los_timer.c 

S_UPPER_SRCS += \
../LiteOS/kernel/arch/risc-v/V4A/gcc/los_dispatch.S \
../LiteOS/kernel/arch/risc-v/V4A/gcc/los_exc.S 

OBJS += \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_context.o \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_dispatch.o \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_exc.o \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_interrupt.o \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_timer.o 

S_UPPER_DEPS += \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_dispatch.d \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_exc.d 

C_DEPS += \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_context.d \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_interrupt.d \
./LiteOS/kernel/arch/risc-v/V4A/gcc/los_timer.d 


# Each subdirectory must supply rules for building sources it contributes
LiteOS/kernel/arch/risc-v/V4A/gcc/%.o: ../LiteOS/kernel/arch/risc-v/V4A/gcc/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused  -g -I"../Debug" -I"../Core" -I"../User" -I"../Peripheral/inc" -I"../LiteOS/components/backtrace" -I"../LiteOS/components/cpup" -I"../LiteOS/components/power" -I"../LiteOS/third_party/bounds_checking_function/include" -I"../LiteOS/third_party/bounds_checking_function/src" -I"../LiteOS/utils/internal" -I"../LiteOS/utils" -I"../LiteOS/kernel/arch/include" -I"../LiteOS/kernel/arch/risc-v/V4A/gcc" -I"../LiteOS/kernel/include" -I"../LiteOS/kernel/src" -I"../LiteOS/kernel/src/mm" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
LiteOS/kernel/arch/risc-v/V4A/gcc/%.o: ../LiteOS/kernel/arch/risc-v/V4A/gcc/%.S
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused  -g -x assembler-with-cpp -I"../Startup" -I"../LiteOS/components/backtrace" -I"../LiteOS/components/cpup" -I"../LiteOS/components/power" -I"../LiteOS/third_party/bounds_checking_function/include" -I"../LiteOS/third_party/bounds_checking_function/src" -I"../LiteOS/utils/internal" -I"../LiteOS/utils" -I"../LiteOS/kernel/arch/include" -I"../LiteOS/kernel/arch/risc-v/V4A/gcc" -I"../LiteOS/kernel/include" -I"../LiteOS/kernel/src" -I"../LiteOS/kernel/src/mm" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

