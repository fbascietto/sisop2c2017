################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../almacenamientoFinal.c \
../funcionesWorker.c \
../interface.c \
../interfaceWorkerMaster.c \
../interfaceWorkerWorker.c \
../reduccionGlobal.c \
../reduccionLocal.c \
../transformacion.c \
../worker.c 

OBJS += \
./almacenamientoFinal.o \
./funcionesWorker.o \
./interface.o \
./interfaceWorkerMaster.o \
./interfaceWorkerWorker.o \
./reduccionGlobal.o \
./reduccionLocal.o \
./transformacion.o \
./worker.o 

C_DEPS += \
./almacenamientoFinal.d \
./funcionesWorker.d \
./interface.d \
./interfaceWorkerMaster.d \
./interfaceWorkerWorker.d \
./reduccionGlobal.d \
./reduccionLocal.d \
./transformacion.d \
./worker.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


