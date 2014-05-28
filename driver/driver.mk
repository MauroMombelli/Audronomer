# List of all the ChibiOS/RT HAL files, there is no need to remove the files
# from this list, you can disable parts of the HAL by editing halconf.h.
DRIVERSRC = ${CHIBIOS}/driver/src/lsm303dlh.c \
			${CHIBIOS}/driver/src/l3g4200d.c

# Required include directories
DRIVERINC = ${CHIBIOS}/driver/include
