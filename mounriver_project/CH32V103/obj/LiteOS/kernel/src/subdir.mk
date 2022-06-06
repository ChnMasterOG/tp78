################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LiteOS/kernel/src/los_event.c \
../LiteOS/kernel/src/los_init.c \
../LiteOS/kernel/src/los_mux.c \
../LiteOS/kernel/src/los_queue.c \
../LiteOS/kernel/src/los_sched.c \
../LiteOS/kernel/src/los_sem.c \
../LiteOS/kernel/src/los_sortlink.c \
../LiteOS/kernel/src/los_swtmr.c \
../LiteOS/kernel/src/los_task.c \
../LiteOS/kernel/src/los_tick.c 

OBJS += \
./LiteOS/kernel/src/los_event.o \
./LiteOS/kernel/src/los_init.o \
./LiteOS/kernel/src/los_mux.o \
./LiteOS/kernel/src/los_queue.o \
./LiteOS/kernel/src/los_sched.o \
./LiteOS/kernel/src/los_sem.o \
./LiteOS/kernel/src/los_sortlink.o \
./LiteOS/kernel/src/los_swtmr.o \
./LiteOS/kernel/src/los_task.o \
./LiteOS/kernel/src/los_tick.o 

C_DEPS += \
./LiteOS/kernel/src/los_event.d \
./LiteOS/kernel/src/los_init.d \
./LiteOS/kernel/src/los_mux.d \
./LiteOS/kernel/src/los_queue.d \
./LiteOS/kernel/src/los_sched.d \
./LiteOS/kernel/src/los_sem.d \
./LiteOS/kernel/src/los_sortlink.d \
./LiteOS/kernel/src/los_swtmr.d \
./LiteOS/kernel/src/los_task.d \
./LiteOS/kernel/src/los_tick.d 


# Each subdirectory must supply rules for building sources it contributes
LiteOS/kernel/src/%.o: ../LiteOS/kernel/src/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused  -g -I"../Debug" -I"../Core" -I"../User" -I"../Peripheral/inc" -I"../LiteOS/components/backtrace" -I"../LiteOS/components/cpup" -I"../LiteOS/components/power" -I"../LiteOS/third_party/bounds_checking_function/include" -I"../LiteOS/third_party/bounds_checking_function/src" -I"../LiteOS/utils/internal" -I"../LiteOS/utils" -I"../LiteOS/kernel/arch/include" -I"../LiteOS/kernel/arch/risc-v/V4A/gcc" -I"../LiteOS/kernel/include" -I"../LiteOS/kernel/src" -I"../LiteOS/kernel/src/mm" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

