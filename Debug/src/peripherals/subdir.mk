################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/peripherals/AnalogIn.c \
../src/peripherals/Digital.c \
../src/peripherals/DigitalIn.c \
../src/peripherals/DigitalOut.c \
../src/peripherals/HardwareTimer.c \
../src/peripherals/I2C.c \
../src/peripherals/InterruptIn.c \
../src/peripherals/PWM.c \
../src/peripherals/Serial.c \
../src/peripherals/SoftwareTimer.c 

OBJS += \
./src/peripherals/AnalogIn.o \
./src/peripherals/Digital.o \
./src/peripherals/DigitalIn.o \
./src/peripherals/DigitalOut.o \
./src/peripherals/HardwareTimer.o \
./src/peripherals/I2C.o \
./src/peripherals/InterruptIn.o \
./src/peripherals/PWM.o \
./src/peripherals/Serial.o \
./src/peripherals/SoftwareTimer.o 

C_DEPS += \
./src/peripherals/AnalogIn.d \
./src/peripherals/Digital.d \
./src/peripherals/DigitalIn.d \
./src/peripherals/DigitalOut.d \
./src/peripherals/HardwareTimer.d \
./src/peripherals/I2C.d \
./src/peripherals/InterruptIn.d \
./src/peripherals/PWM.d \
./src/peripherals/Serial.d \
./src/peripherals/SoftwareTimer.d 


# Each subdirectory must supply rules for building sources it contributes
src/peripherals/%.o: ../src/peripherals/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DTARGET_LPC111X -DDEBUG -D__CODE_RED -I"C:\Users\David Alain\Documents\LPCXpresso_4.2.0_264\workspace\OpenLPC_C\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


