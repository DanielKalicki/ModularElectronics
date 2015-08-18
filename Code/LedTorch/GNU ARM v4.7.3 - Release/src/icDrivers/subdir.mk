################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/icDrivers/ADP1650.c 

OBJS += \
./src/icDrivers/ADP1650.o 

C_DEPS += \
./src/icDrivers/ADP1650.d 


# Each subdirectory must supply rules for building sources it contributes
src/icDrivers/ADP1650.o: ../src/icDrivers/ADP1650.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb '-DEFM32ZG108F32=1' '-DNDEBUG=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/icDrivers/ADP1650.d" -MT"src/icDrivers/ADP1650.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


