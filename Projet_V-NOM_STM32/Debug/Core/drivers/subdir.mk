################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/driver_ADXL343.c \
../Core/drivers/driver_GP2Y0A41SK0F.c \
../Core/drivers/driver_Motor.c \
../Core/drivers/driver_YLIDARX2.c 

OBJS += \
./Core/drivers/driver_ADXL343.o \
./Core/drivers/driver_GP2Y0A41SK0F.o \
./Core/drivers/driver_Motor.o \
./Core/drivers/driver_YLIDARX2.o 

C_DEPS += \
./Core/drivers/driver_ADXL343.d \
./Core/drivers/driver_GP2Y0A41SK0F.d \
./Core/drivers/driver_Motor.d \
./Core/drivers/driver_YLIDARX2.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/%.o Core/drivers/%.su Core/drivers/%.cyclo: ../Core/drivers/%.c Core/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-drivers

clean-Core-2f-drivers:
	-$(RM) ./Core/drivers/driver_ADXL343.cyclo ./Core/drivers/driver_ADXL343.d ./Core/drivers/driver_ADXL343.o ./Core/drivers/driver_ADXL343.su ./Core/drivers/driver_GP2Y0A41SK0F.cyclo ./Core/drivers/driver_GP2Y0A41SK0F.d ./Core/drivers/driver_GP2Y0A41SK0F.o ./Core/drivers/driver_GP2Y0A41SK0F.su ./Core/drivers/driver_Motor.cyclo ./Core/drivers/driver_Motor.d ./Core/drivers/driver_Motor.o ./Core/drivers/driver_Motor.su ./Core/drivers/driver_YLIDARX2.cyclo ./Core/drivers/driver_YLIDARX2.d ./Core/drivers/driver_YLIDARX2.o ./Core/drivers/driver_YLIDARX2.su

.PHONY: clean-Core-2f-drivers

