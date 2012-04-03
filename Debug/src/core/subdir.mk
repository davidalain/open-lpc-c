################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/core_cm0.c \
../src/core/core_cm3.c \
../src/core/startup.c \
../src/core/system_LPC11xx.c \
../src/core/system_LPC13xx.c \
../src/core/system_LPC17xx.c 

OBJS += \
./src/core/core_cm0.o \
./src/core/core_cm3.o \
./src/core/startup.o \
./src/core/system_LPC11xx.o \
./src/core/system_LPC13xx.o \
./src/core/system_LPC17xx.o 

C_DEPS += \
./src/core/core_cm0.d \
./src/core/core_cm3.d \
./src/core/startup.d \
./src/core/system_LPC11xx.d \
./src/core/system_LPC13xx.d \
./src/core/system_LPC17xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/%.o: ../src/core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DTARGET_LPC111X -DDEBUG -D__CODE_RED -I"C:\Users\David Alain\Documents\LPCXpresso_4.2.0_264\workspace\OpenLPC_C\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


