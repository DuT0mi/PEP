################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include drivers/subdir.mk
-include CMSIS/EFM32GG/subdir.mk
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
all: Snake_homework.axf

# Tool invocations
Snake_homework.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "Snake_homework.ld" -Xlinker --gc-sections -Xlinker -Map="Snake_homework.map" --specs=nano.specs -o Snake_homework.axf "./CMSIS/EFM32GG/startup_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./drivers/segmentlcd.o" "./emlib/em_cmu.o" "./emlib/em_gpio.o" "./emlib/em_lcd.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./src/main.o" "./src/segmentlcd_individual.o" "./src/snake.o" "./src/timer.o" "./src/uart.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Snake_homework.hex'
	arm-none-eabi-objcopy -O ihex "Snake_homework.axf" "Snake_homework.hex"
	@echo ' '

	@echo 'Building bin file: Snake_homework.bin'
	arm-none-eabi-objcopy -O binary "Snake_homework.axf" "Snake_homework.bin"
	@echo ' '

	@echo 'Building s37 file: Snake_homework.s37'
	arm-none-eabi-objcopy -O srec "Snake_homework.axf" "Snake_homework.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Snake_homework.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Snake_homework.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
