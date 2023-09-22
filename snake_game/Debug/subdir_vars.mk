################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../MSP_EXP432P401R_TIRTOS.cmd 

SYSCFG_SRCS += \
../adcsinglechannel.syscfg 

C_SRCS += \
./syscfg/ti_drivers_config.c \
../bsp.c \
../main_tirtos.c \
../protectedlcd.c 

GEN_FILES += \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./syscfg/ti_drivers_config.d \
./bsp.d \
./main_tirtos.d \
./protectedlcd.d 

OBJS += \
./syscfg/ti_drivers_config.obj \
./bsp.obj \
./main_tirtos.obj \
./protectedlcd.obj 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/syscfg_c.rov.xs 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"syscfg\ti_drivers_config.obj" \
"bsp.obj" \
"main_tirtos.obj" \
"protectedlcd.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\syscfg_c.rov.xs" 

C_DEPS__QUOTED += \
"syscfg\ti_drivers_config.d" \
"bsp.d" \
"main_tirtos.d" \
"protectedlcd.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_drivers_config.c" 

SYSCFG_SRCS__QUOTED += \
"../adcsinglechannel.syscfg" 

C_SRCS__QUOTED += \
"./syscfg/ti_drivers_config.c" \
"../bsp.c" \
"../main_tirtos.c" \
"../protectedlcd.c" 


