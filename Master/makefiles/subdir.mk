################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../funcionesmaster.c \
../interface.c \
../interfaceMaster.c \
../interfaceWorker.c \
../master.c 

OBJS += \
./funcionesmaster.o \
./interface.o \
./interfaceMaster.o \
./interfaceWorker.o \
./master.o 

C_DEPS += \
./funcionesmaster.d \
./interface.d \
./interfaceMaster.d \
./interfaceWorker.d \
./master.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


