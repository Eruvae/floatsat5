################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include wifi/drivers/wf121/subdir.mk
-include wifi/drivers/esp8266/espdriver/subdir.mk
-include wifi/drivers/esp8266/subdir.mk
-include wifi/subdir.mk
-include SpyderCrane_HW_Lib/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
floatsat5-obc.hex \

SECONDARY_SIZE += \
floatsat5-obc.siz \


# All Target
all: floatsat5-obc.elf secondary-outputs

# Tool invocations
floatsat5-obc.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "C:\Users\TobiZ\Eclipse Luna Floatsat\Workspace\rodos\src\bare-metal\stm32f4\scripts\stm32_flash.ld" -nostartfiles -nodefaultlibs -nostdlib -Xlinker --gc-sections -L"C:\Users\TobiZ\Eclipse Luna Floatsat\Workspace\rodos\Debug_STM32F4" -L"C:\Users\TobiZ\Eclipse Luna Floatsat\Workspace\support_libs\Debug_STM32F4" -Wl,-Map,"floatsat5-obc.map" -o "floatsat5-obc.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

floatsat5-obc.hex: floatsat5-obc.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "floatsat5-obc.elf"  "floatsat5-obc.hex"
	@echo 'Finished building: $@'
	@echo ' '

floatsat5-obc.siz: floatsat5-obc.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "floatsat5-obc.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) floatsat5-obc.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
