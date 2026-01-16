################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/core_riscv.c 

C_DEPS += \
./Core/core_riscv.d 

OBJS += \
./Core/core_riscv.o 


EXPANDS += \
./Core/core_riscv.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
Core/%.o: ../Core/%.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/Debug" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/Core" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/Peripheral/inc" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User/Inc" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User/inverter" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User/inverter/Inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

