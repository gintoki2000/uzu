################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/behaviour_tree.c \
../src/components.c \
../src/entity_factory.c \
../src/generator.c \
../src/graph.c \
../src/inventory.c \
../src/map.c \
../src/path_finder.c \
../src/resources.c \
../src/uzu.c 

OBJS += \
./src/behaviour_tree.o \
./src/components.o \
./src/entity_factory.o \
./src/generator.o \
./src/graph.o \
./src/inventory.o \
./src/map.o \
./src/path_finder.o \
./src/resources.o \
./src/uzu.o 

C_DEPS += \
./src/behaviour_tree.d \
./src/components.d \
./src/entity_factory.d \
./src/generator.d \
./src/graph.d \
./src/inventory.d \
./src/map.d \
./src/path_finder.d \
./src/resources.d \
./src/uzu.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


