################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
myproject.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
myproject.list \


# All Target
all: myproject.elf secondary-outputs

# Tool invocations
myproject.elf: $(OBJS) $(USER_OBJS) D:\EmbeddedSystem-prog-on-ARMcortex-M3M4_mc\cortexmx\myproject\STM32F410RBTX_FLASH.ld
	arm-none-eabi-gcc -o "myproject.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\EmbeddedSystem-prog-on-ARMcortex-M3M4_mc\cortexmx\myproject\STM32F410RBTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="myproject.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

myproject.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "myproject.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
