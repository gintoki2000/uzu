################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ecs/common.c \
../src/ecs/ecs.c \
../src/ecs/entity_pool.c \
../src/ecs/pool.c 

OBJS += \
./src/ecs/common.o \
./src/ecs/ecs.o \
./src/ecs/entity_pool.o \
./src/ecs/pool.o 

C_DEPS += \
./src/ecs/common.d \
./src/ecs/ecs.d \
./src/ecs/entity_pool.d \
./src/ecs/pool.d 


# Each subdirectory must supply rules for building sources it contributes
src/ecs/%.o: ../src/ecs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


