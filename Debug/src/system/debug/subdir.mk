################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/system/debug/draw_path.c 

OBJS += \
./src/system/debug/draw_path.o 

C_DEPS += \
./src/system/debug/draw_path.d 


# Each subdirectory must supply rules for building sources it contributes
src/system/debug/%.o: ../src/system/debug/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


