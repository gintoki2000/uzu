################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ai/btt_find_player_location.c \
../src/ai/btt_find_random_location.c \
../src/ai/btt_follow_path.c \
../src/ai/btt_move_to.c \
../src/ai/btt_wait.c 

OBJS += \
./src/ai/btt_find_player_location.o \
./src/ai/btt_find_random_location.o \
./src/ai/btt_follow_path.o \
./src/ai/btt_move_to.o \
./src/ai/btt_wait.o 

C_DEPS += \
./src/ai/btt_find_player_location.d \
./src/ai/btt_find_random_location.d \
./src/ai/btt_follow_path.d \
./src/ai/btt_move_to.d \
./src/ai/btt_wait.d 


# Each subdirectory must supply rules for building sources it contributes
src/ai/%.o: ../src/ai/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


