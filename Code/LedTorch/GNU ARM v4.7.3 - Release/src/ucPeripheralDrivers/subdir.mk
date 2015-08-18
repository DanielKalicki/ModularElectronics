################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ucPeripheralDrivers/RTC_.c \
../src/ucPeripheralDrivers/i2c_connection.c \
../src/ucPeripheralDrivers/spi.c \
../src/ucPeripheralDrivers/spi_connection.c \
../src/ucPeripheralDrivers/uart_connection.c 

OBJS += \
./src/ucPeripheralDrivers/RTC_.o \
./src/ucPeripheralDrivers/i2c_connection.o \
./src/ucPeripheralDrivers/spi.o \
./src/ucPeripheralDrivers/spi_connection.o \
./src/ucPeripheralDrivers/uart_connection.o 

C_DEPS += \
./src/ucPeripheralDrivers/RTC_.d \
./src/ucPeripheralDrivers/i2c_connection.d \
./src/ucPeripheralDrivers/spi.d \
./src/ucPeripheralDrivers/spi_connection.d \
./src/ucPeripheralDrivers/uart_connection.d 


# Each subdirectory must supply rules for building sources it contributes
src/ucPeripheralDrivers/RTC_.o: ../src/ucPeripheralDrivers/RTC_.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb '-DEFM32ZG108F32=1' '-DNDEBUG=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/RTC_.d" -MT"src/ucPeripheralDrivers/RTC_.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/i2c_connection.o: ../src/ucPeripheralDrivers/i2c_connection.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb '-DEFM32ZG108F32=1' '-DNDEBUG=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/i2c_connection.d" -MT"src/ucPeripheralDrivers/i2c_connection.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/spi.o: ../src/ucPeripheralDrivers/spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb '-DEFM32ZG108F32=1' '-DNDEBUG=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/spi.d" -MT"src/ucPeripheralDrivers/spi.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/spi_connection.o: ../src/ucPeripheralDrivers/spi_connection.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb '-DEFM32ZG108F32=1' '-DNDEBUG=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/spi_connection.d" -MT"src/ucPeripheralDrivers/spi_connection.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/uart_connection.o: ../src/ucPeripheralDrivers/uart_connection.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb '-DEFM32ZG108F32=1' '-DNDEBUG=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/uart_connection.d" -MT"src/ucPeripheralDrivers/uart_connection.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


