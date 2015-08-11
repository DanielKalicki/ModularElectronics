################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -T "UserInterface.ld" -Xlinker --gc-sections -Xlinker -Map="UserInterface.map" --specs=nano.specs -o UserInterface.axf "./src/empty_project.o" "./emlib/em_system.o" "./CMSIS/efm32zg/startup_gcc_efm32zg.o" "./CMSIS/efm32zg/system_efm32zg.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
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
