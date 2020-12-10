################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/engine/engine.c \
../src/engine/keyboard.c 

OBJS += \
./src/engine/engine.o \
./src/engine/keyboard.o 

C_DEPS += \
./src/engine/engine.d \
./src/engine/keyboard.d 


# Each subdirectory must supply rules for building sources it contributes
src/engine/%.o: ../src/engine/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


