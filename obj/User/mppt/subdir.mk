################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/mppt/app.c \
../User/mppt/faults.c \
../User/mppt/hardware.c \
../User/mppt/modbus_slave.c \
../User/mppt/mppt.c 

C_DEPS += \
./User/mppt/app.d \
./User/mppt/faults.d \
./User/mppt/hardware.d \
./User/mppt/modbus_slave.d \
./User/mppt/mppt.d 

OBJS += \
./User/mppt/app.o \
./User/mppt/faults.o \
./User/mppt/hardware.o \
./User/mppt/modbus_slave.o \
./User/mppt/mppt.o 


EXPANDS += \
./User/mppt/app.c.234r.expand \
./User/mppt/faults.c.234r.expand \
./User/mppt/hardware.c.234r.expand \
./User/mppt/modbus_slave.c.234r.expand \
./User/mppt/mppt.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/mppt/%.o: ../User/mppt/%.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"/home/ubuntu12345/inverter/mounriver-studio-projects/mppt-ch32/Debug" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/mppt-ch32/Core" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/mppt-ch32/User" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/mppt-ch32/Peripheral/inc" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/mppt-ch32/User/Inc" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/mppt-ch32/User/mppt" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

