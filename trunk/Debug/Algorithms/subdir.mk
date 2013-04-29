################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../Algorithms/random-singleton.o 

CPP_SRCS += \
../Algorithms/random-singleton.cpp 

OBJS += \
./Algorithms/random-singleton.o 

CPP_DEPS += \
./Algorithms/random-singleton.d 


# Each subdirectory must supply rules for building sources it contributes
Algorithms/%.o: ../Algorithms/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


