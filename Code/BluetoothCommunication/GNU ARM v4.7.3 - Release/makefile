################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/ucPeripheralDrivers/subdir.mk
-include src/icDrivers/subdir.mk
-include src/communicationDrivers/subdir.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/efm32zg/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: BluetoothCommunication.axf

# Tool invocations
BluetoothCommunication.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -T "BluetoothCommunication.ld" -Xlinker --gc-sections -Xlinker -Map="BluetoothCommunication.map" --specs=nano.specs -o BluetoothCommunication.axf "./src/ucPeripheralDrivers/PWM_.o" "./src/ucPeripheralDrivers/RTC_.o" "./src/ucPeripheralDrivers/i2c_connection.o" "./src/ucPeripheralDrivers/leuart_connection.o" "./src/ucPeripheralDrivers/spi.o" "./src/ucPeripheralDrivers/spi_connection.o" "./src/ucPeripheralDrivers/timer_.o" "./src/ucPeripheralDrivers/uart_connection.o" "./src/main.o" "./src/icDrivers/HM11.o" "./src/communicationDrivers/mod_comm.o" "./emlib/em_acmp.o" "./emlib/em_adc.o" "./emlib/em_aes.o" "./emlib/em_assert.o" "./emlib/em_burtc.o" "./emlib/em_cmu.o" "./emlib/em_dac.o" "./emlib/em_dbg.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_idac.o" "./emlib/em_int.o" "./emlib/em_lcd.o" "./emlib/em_lesense.o" "./emlib/em_letimer.o" "./emlib/em_leuart.o" "./emlib/em_mpu.o" "./emlib/em_msc.o" "./emlib/em_opamp.o" "./emlib/em_pcnt.o" "./emlib/em_prs.o" "./emlib/em_rmu.o" "./emlib/em_rtc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_vcmp.o" "./emlib/em_wdog.o" "./CMSIS/efm32zg/startup_gcc_efm32zg.o" "./CMSIS/efm32zg/system_efm32zg.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: BluetoothCommunication.hex'
	arm-none-eabi-objcopy -O ihex "BluetoothCommunication.axf" "BluetoothCommunication.hex"
	@echo ' '

	@echo 'Building bin file: BluetoothCommunication.bin'
	arm-none-eabi-objcopy -O binary "BluetoothCommunication.axf" "BluetoothCommunication.bin"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "BluetoothCommunication.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) BluetoothCommunication.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
