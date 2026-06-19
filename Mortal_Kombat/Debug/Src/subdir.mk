################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ADC.c \
../Src/DMA.c \
../Src/TIM.c \
../Src/controller_GPIO.c \
../Src/display.c \
../Src/game.c \
../Src/main.c \
../Src/mortalKombatLogo.c \
../Src/physics.c \
../Src/player.c \
../Src/sprites.c \
../Src/ssd1306_fonts.c \
../Src/ssd1306_nohal.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_clock.c \
../Src/themeSong.c 

OBJS += \
./Src/ADC.o \
./Src/DMA.o \
./Src/TIM.o \
./Src/controller_GPIO.o \
./Src/display.o \
./Src/game.o \
./Src/main.o \
./Src/mortalKombatLogo.o \
./Src/physics.o \
./Src/player.o \
./Src/sprites.o \
./Src/ssd1306_fonts.o \
./Src/ssd1306_nohal.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_clock.o \
./Src/themeSong.o 

C_DEPS += \
./Src/ADC.d \
./Src/DMA.d \
./Src/TIM.d \
./Src/controller_GPIO.d \
./Src/display.d \
./Src/game.d \
./Src/main.d \
./Src/mortalKombatLogo.d \
./Src/physics.d \
./Src/player.d \
./Src/sprites.d \
./Src/ssd1306_fonts.d \
./Src/ssd1306_nohal.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_clock.d \
./Src/themeSong.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32L4 -DSTM32 -DNUCLEO_L476RG -DSTM32L476RGTx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/ADC.cyclo ./Src/ADC.d ./Src/ADC.o ./Src/ADC.su ./Src/DMA.cyclo ./Src/DMA.d ./Src/DMA.o ./Src/DMA.su ./Src/TIM.cyclo ./Src/TIM.d ./Src/TIM.o ./Src/TIM.su ./Src/controller_GPIO.cyclo ./Src/controller_GPIO.d ./Src/controller_GPIO.o ./Src/controller_GPIO.su ./Src/display.cyclo ./Src/display.d ./Src/display.o ./Src/display.su ./Src/game.cyclo ./Src/game.d ./Src/game.o ./Src/game.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/mortalKombatLogo.cyclo ./Src/mortalKombatLogo.d ./Src/mortalKombatLogo.o ./Src/mortalKombatLogo.su ./Src/physics.cyclo ./Src/physics.d ./Src/physics.o ./Src/physics.su ./Src/player.cyclo ./Src/player.d ./Src/player.o ./Src/player.su ./Src/sprites.cyclo ./Src/sprites.d ./Src/sprites.o ./Src/sprites.su ./Src/ssd1306_fonts.cyclo ./Src/ssd1306_fonts.d ./Src/ssd1306_fonts.o ./Src/ssd1306_fonts.su ./Src/ssd1306_nohal.cyclo ./Src/ssd1306_nohal.d ./Src/ssd1306_nohal.o ./Src/ssd1306_nohal.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/system_clock.cyclo ./Src/system_clock.d ./Src/system_clock.o ./Src/system_clock.su ./Src/themeSong.cyclo ./Src/themeSong.d ./Src/themeSong.o ./Src/themeSong.su

.PHONY: clean-Src

