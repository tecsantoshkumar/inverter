################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Debug/debug.c 

C_DEPS += \
./Debug/debug.d 

OBJS += \
./Debug/debug.o 


EXPANDS += \
./Debug/debug.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
Debug/%.o: ../Debug/%.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/Debug" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/Core" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/Peripheral/inc" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User/Inc" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User/inverter" -I"/home/ubuntu12345/inverter/mounriver-studio-projects/inverter-ch32/User/inverter/Inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

