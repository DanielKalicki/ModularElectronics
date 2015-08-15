################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/ucPeripheralDrivers/subdir.mk
-include src/icDrivers/ADP8866/subdir.mk
-include src/icDrivers/AD7147/subdir.mk
-include src/icDrivers/subdir.mk
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
all: UserInterface.axf

# Tool invocations
UserInterface.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -T "UserInterface.ld" -Xlinker --gc-sections -Xlinker -Map="UserInterface.map" --specs=nano.specs -o UserInterface.axf "./src/ucPeripheralDrivers/PWM_.o" "./src/ucPeripheralDrivers/RTC_.o" "./src/ucPeripheralDrivers/i2c_connection.o" "./src/ucPeripheralDrivers/leuart_connection.o" "./src/ucPeripheralDrivers/spi.o" "./src/ucPeripheralDrivers/spi_connection.o" "./src/ucPeripheralDrivers/uart_connection.o" "./src/main.o" "./src/icDrivers/BD6210.o" "./src/icDrivers/ADP8866/ADP8866_reg.o" "./src/icDrivers/AD7147/AD7147_reg.o" "./emlib/em_acmp.o" "./emlib/em_adc.o" "./emlib/em_aes.o" "./emlib/em_assert.o" "./emlib/em_burtc.o" "./emlib/em_cmu.o" "./emlib/em_dac.o" "./emlib/em_dbg.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_idac.o" "./emlib/em_int.o" "./emlib/em_lcd.o" "./emlib/em_lesense.o" "./emlib/em_letimer.o" "./emlib/em_leuart.o" "./emlib/em_mpu.o" "./emlib/em_msc.o" "./emlib/em_opamp.o" "./emlib/em_pcnt.o" "./emlib/em_prs.o" "./emlib/em_rmu.o" "./emlib/em_rtc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_vcmp.o" "./emlib/em_wdog.o" "./CMSIS/efm32zg/startup_gcc_efm32zg.o" "./CMSIS/efm32zg/system_efm32zg.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: UserInterface.hex'
	arm-none-eabi-objcopy -O ihex "UserInterface.axf" "UserInterface.hex"
	@echo ' '

	@echo 'Building bin file: UserInterface.bin'
	arm-none-eabi-objcopy -O binary "UserInterface.axf" "UserInterface.bin"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "UserInterface.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) UserInterface.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
