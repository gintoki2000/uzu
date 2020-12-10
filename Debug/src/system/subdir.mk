################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/system/action_execution_system.c \
../src/system/ai_system.c \
../src/system/animator_controller_system.c \
../src/system/animator_system.c \
../src/system/camera_following_system.c \
../src/system/collision_filter.c \
../src/system/collision_system.c \
../src/system/draw_system.c \
../src/system/draw_target.c \
../src/system/drawing_heal_bar.c \
../src/system/drawing_hitbox_system.c \
../src/system/drop_system.c \
../src/system/health_system.c \
../src/system/late_destroying_system.c \
../src/system/life_span_system.c \
../src/system/map_collision_system.c \
../src/system/mediator.c \
../src/system/motion_system.c \
../src/system/pickup_system.c \
../src/system/player_controller_system.c \
../src/system/sound_sytem.c \
../src/system/swinging_system.c \
../src/system/sync_eqm_system.c \
../src/system/weapon_dealing_damage_system.c 

OBJS += \
./src/system/action_execution_system.o \
./src/system/ai_system.o \
./src/system/animator_controller_system.o \
./src/system/animator_system.o \
./src/system/camera_following_system.o \
./src/system/collision_filter.o \
./src/system/collision_system.o \
./src/system/draw_system.o \
./src/system/draw_target.o \
./src/system/drawing_heal_bar.o \
./src/system/drawing_hitbox_system.o \
./src/system/drop_system.o \
./src/system/health_system.o \
./src/system/late_destroying_system.o \
./src/system/life_span_system.o \
./src/system/map_collision_system.o \
./src/system/mediator.o \
./src/system/motion_system.o \
./src/system/pickup_system.o \
./src/system/player_controller_system.o \
./src/system/sound_sytem.o \
./src/system/swinging_system.o \
./src/system/sync_eqm_system.o \
./src/system/weapon_dealing_damage_system.o 

C_DEPS += \
./src/system/action_execution_system.d \
./src/system/ai_system.d \
./src/system/animator_controller_system.d \
./src/system/animator_system.d \
./src/system/camera_following_system.d \
./src/system/collision_filter.d \
./src/system/collision_system.d \
./src/system/draw_system.d \
./src/system/draw_target.d \
./src/system/drawing_heal_bar.d \
./src/system/drawing_hitbox_system.d \
./src/system/drop_system.d \
./src/system/health_system.d \
./src/system/late_destroying_system.d \
./src/system/life_span_system.d \
./src/system/map_collision_system.d \
./src/system/mediator.d \
./src/system/motion_system.d \
./src/system/pickup_system.d \
./src/system/player_controller_system.d \
./src/system/sound_sytem.d \
./src/system/swinging_system.d \
./src/system/sync_eqm_system.d \
./src/system/weapon_dealing_damage_system.d 


# Each subdirectory must supply rules for building sources it contributes
src/system/%.o: ../src/system/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


