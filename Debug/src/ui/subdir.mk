################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ui/health_bar.c \
../src/ui/window.c 

OBJS += \
./src/ui/health_bar.o \
./src/ui/window.o 

C_DEPS += \
./src/ui/health_bar.d \
./src/ui/window.d 


# Each subdirectory must supply rules for building sources it contributes
src/ui/%.o: ../src/ui/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


