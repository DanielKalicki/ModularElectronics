################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ucPeripheralDrivers/PWM_.c \
../src/ucPeripheralDrivers/RTC_.c \
../src/ucPeripheralDrivers/i2c_connection.c \
../src/ucPeripheralDrivers/leuart_connection.c \
../src/ucPeripheralDrivers/spi.c \
../src/ucPeripheralDrivers/spi_connection.c \
../src/ucPeripheralDrivers/timer_.c \
../src/ucPeripheralDrivers/uart_connection.c 

OBJS += \
./src/ucPeripheralDrivers/PWM_.o \
./src/ucPeripheralDrivers/RTC_.o \
./src/ucPeripheralDrivers/i2c_connection.o \
./src/ucPeripheralDrivers/leuart_connection.o \
./src/ucPeripheralDrivers/spi.o \
./src/ucPeripheralDrivers/spi_connection.o \
./src/ucPeripheralDrivers/timer_.o \
./src/ucPeripheralDrivers/uart_connection.o 

C_DEPS += \
./src/ucPeripheralDrivers/PWM_.d \
./src/ucPeripheralDrivers/RTC_.d \
./src/ucPeripheralDrivers/i2c_connection.d \
./src/ucPeripheralDrivers/leuart_connection.d \
./src/ucPeripheralDrivers/spi.d \
./src/ucPeripheralDrivers/spi_connection.d \
./src/ucPeripheralDrivers/timer_.d \
./src/ucPeripheralDrivers/uart_connection.d 


# Each subdirectory must supply rules for building sources it contributes
src/ucPeripheralDrivers/PWM_.o: ../src/ucPeripheralDrivers/PWM_.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/PWM_.d" -MT"src/ucPeripheralDrivers/PWM_.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/RTC_.o: ../src/ucPeripheralDrivers/RTC_.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/RTC_.d" -MT"src/ucPeripheralDrivers/RTC_.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/i2c_connection.o: ../src/ucPeripheralDrivers/i2c_connection.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/i2c_connection.d" -MT"src/ucPeripheralDrivers/i2c_connection.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/leuart_connection.o: ../src/ucPeripheralDrivers/leuart_connection.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/leuart_connection.d" -MT"src/ucPeripheralDrivers/leuart_connection.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/spi.o: ../src/ucPeripheralDrivers/spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/spi.d" -MT"src/ucPeripheralDrivers/spi.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/spi_connection.o: ../src/ucPeripheralDrivers/spi_connection.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/spi_connection.d" -MT"src/ucPeripheralDrivers/spi_connection.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/timer_.o: ../src/ucPeripheralDrivers/timer_.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/timer_.d" -MT"src/ucPeripheralDrivers/timer_.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ucPeripheralDrivers/uart_connection.o: ../src/ucPeripheralDrivers/uart_connection.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb '-DDEBUG=1' '-DEFM32ZG108F32=1' -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/CMSIS/Include" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/bsp" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/emlib/inc" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/kits/common/drivers" -I"D:\SiliconLabs\SimplicityStudio\v2\developer\sdks\efm32\v2/Device/SiliconLabs/EFM32ZG/Include" -O3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"src/ucPeripheralDrivers/uart_connection.d" -MT"src/ucPeripheralDrivers/uart_connection.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


