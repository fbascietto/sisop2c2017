################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ejemploInicializacionNodos.c \
../funcionesyama.c \
../interface.c \
../interfaceMaster.c \
../job.c \
../prePlanificacion.c \
../tablaEstados.c \
../yama.c 

OBJS += \
./ejemploInicializacionNodos.o \
./funcionesyama.o \
./interface.o \
./interfaceMaster.o \
./job.o \
./prePlanificacion.o \
./tablaEstados.o \
./yama.o 

C_DEPS += \
./ejemploInicializacionNodos.d \
./funcionesyama.d \
./interface.d \
./interfaceMaster.d \
./job.d \
./prePlanificacion.d \
./tablaEstados.d \
./yama.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


