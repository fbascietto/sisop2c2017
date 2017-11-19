################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../interfaceWorkerMaster.c \
../main.c \
../serializacion.c \
../sockets.c 

OBJS += \
./interfaceWorkerMaster.o \
./main.o \
./serializacion.o \
./sockets.o 

C_DEPS += \
./interfaceWorkerMaster.d \
./main.d \
./serializacion.d \
./sockets.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


