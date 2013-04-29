################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Examples/ccArea.cpp \
../Examples/example.cpp \
../Examples/exemple_simple.cpp 

OBJS += \
./Examples/ccArea.o \
./Examples/example.o \
./Examples/exemple_simple.o 

CPP_DEPS += \
./Examples/ccArea.d \
./Examples/example.d \
./Examples/exemple_simple.d 


# Each subdirectory must supply rules for building sources it contributes
Examples/%.o: ../Examples/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


