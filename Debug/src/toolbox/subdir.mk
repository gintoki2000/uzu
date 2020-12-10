################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/toolbox/array.c \
../src/toolbox/heap.c \
../src/toolbox/ptrarray.c \
../src/toolbox/rect.c \
../src/toolbox/rtree.c \
../src/toolbox/signal.c \
../src/toolbox/sprite-sheet.c \
../src/toolbox/sprite.c 

OBJS += \
./src/toolbox/array.o \
./src/toolbox/heap.o \
./src/toolbox/ptrarray.o \
./src/toolbox/rect.o \
./src/toolbox/rtree.o \
./src/toolbox/signal.o \
./src/toolbox/sprite-sheet.o \
./src/toolbox/sprite.o 

C_DEPS += \
./src/toolbox/array.d \
./src/toolbox/heap.d \
./src/toolbox/ptrarray.d \
./src/toolbox/rect.d \
./src/toolbox/rtree.d \
./src/toolbox/signal.d \
./src/toolbox/sprite-sheet.d \
./src/toolbox/sprite.d 


# Each subdirectory must supply rules for building sources it contributes
src/toolbox/%.o: ../src/toolbox/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


