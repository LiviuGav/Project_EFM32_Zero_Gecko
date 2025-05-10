################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include service/subdir.mk
-include emlib/subdir.mk
-include Graphics/subdir.mk
-include Drivers/subdir.mk
-include CMSIS/EFM32ZG/subdir.mk
-include BSP/subdir.mk
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
all: AROBS_2025.axf

# Tool invocations
AROBS_2025.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -T "AROBS_2025.ld" -Xlinker --gc-sections -Xlinker -Map="AROBS_2025.map" --specs=nano.specs -o AROBS_2025.axf "./BSP/bsp_bcc.o" "./BSP/bsp_stk.o" "./CMSIS/EFM32ZG/startup_efm32zg.o" "./CMSIS/EFM32ZG/system_efm32zg.o" "./Drivers/capsense.o" "./Drivers/display.o" "./Drivers/displayls013b7dh03.o" "./Drivers/displaypalemlib.o" "./Drivers/i2cspm.o" "./Drivers/retargetio.o" "./Drivers/retargettextdisplay.o" "./Drivers/si1147_i2c.o" "./Drivers/si114x_algorithm.o" "./Drivers/si114x_functions.o" "./Drivers/si7013.o" "./Drivers/textdisplay.o" "./Drivers/udelay.o" "./Graphics/dmd_display.o" "./Graphics/glib.o" "./Graphics/glib_bitmap.o" "./Graphics/glib_circle.o" "./Graphics/glib_font_normal_8x8.o" "./Graphics/glib_line.o" "./Graphics/glib_rectangle.o" "./Graphics/glib_string.o" "./emlib/em_acmp.o" "./emlib/em_adc.o" "./emlib/em_assert.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_leuart.o" "./emlib/em_rtc.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./service/sl_sleeptimer.o" "./service/sl_sleeptimer_hal_rtc.o" "./src/CP0_CP1.o" "./src/bsp_stk_leds.o" "./src/hand.o" "./src/led.o" "./src/menu.o" "./src/weatherstation.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: AROBS_2025.hex'
	arm-none-eabi-objcopy -O ihex "AROBS_2025.axf" "AROBS_2025.hex"
	@echo ' '

	@echo 'Building bin file: AROBS_2025.bin'
	arm-none-eabi-objcopy -O binary "AROBS_2025.axf" "AROBS_2025.bin"
	@echo ' '

	@echo 'Building s37 file: AROBS_2025.s37'
	arm-none-eabi-objcopy -O srec "AROBS_2025.axf" "AROBS_2025.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "AROBS_2025.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) AROBS_2025.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
