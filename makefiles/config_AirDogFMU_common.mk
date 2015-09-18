#
# Common configuration for all AirDog FMU revisions.
#

#
# Use the configuration's ROMFS, copy the px4iov2 firmware into
# the ROMFS if it's available
#
ROMFS_ROOT	 ?= $(PX4_BASE)/ROMFS/AirDogFMU

# IO Chip
#ROMFS_EXTRA_FILES = $(PX4_BASE)/Images/px4io-v2_default.bin

#
# Board support modules
#
MODULES		+= drivers/bluetooth21
MODULES		+= drivers/boards/AirDogFMU/$(CONFIG_BOARD_REVISION)
MODULES		+= drivers/boards/AirDogFMU/frame_button
MODULES		+= drivers/calibration
MODULES		+= drivers/device
MODULES		+= drivers/frsky_telemetry
MODULES		+= drivers/gps
MODULES		+= drivers/hmc5883spi
MODULES		+= drivers/l3gd20
MODULES		+= drivers/led
MODULES		+= drivers/lsm303d
MODULES		+= drivers/mpu6000
MODULES		+= drivers/ms5611
MODULES		+= drivers/px4io
MODULES		+= drivers/stm32
MODULES		+= drivers/stm32/adc
MODULES		+= drivers/stm32/tone_alarm
MODULES		+= modules/airdog
MODULES		+= modules/bgc
MODULES		+= modules/debug_button
MODULES		+= modules/gpio_tool
MODULES		+= modules/eparam
MODULES		+= modules/indication
MODULES		+= modules/sensors
MODULES		+= drivers/pwm_input
#MODULES		+= drivers/airspeed
#MODULES		+= drivers/blinkm
#MODULES		+= drivers/bluetooth21/tests/bluetooth21_devtest
#MODULES		+= drivers/ets_airspeed
#MODULES		+= drivers/hil
#MODULES		+= drivers/hmc5883
#MODULES		+= drivers/hott/hott_sensors
#MODULES		+= drivers/hott/hott_telemetry
#MODULES		+= drivers/ll905
#MODULES		+= drivers/mb1230serial
#MODULES		+= drivers/mb12xx
#MODULES		+= drivers/meas_airspeed
#MODULES		+= drivers/pca8574
#MODULES		+= drivers/px4flow
#MODULES		+= drivers/px4fmu
#MODULES		+= drivers/rgbled
#MODULES		+= drivers/sf0x

#
# System commands
#
MODULES		+= modules/airdog/calibrator
MODULES		+= systemcmds/config
MODULES		+= systemcmds/dumpfile
MODULES		+= systemcmds/mixer
MODULES		+= systemcmds/mtd
MODULES		+= systemcmds/nshterm
MODULES		+= systemcmds/param
MODULES		+= systemcmds/perf
MODULES		+= systemcmds/preflight_check
MODULES		+= systemcmds/pwm
MODULES		+= systemcmds/reboot
MODULES		+= modules/sensor_validation
MODULES		+= systemcmds/top
MODULES		+= systemcmds/ver
#MODULES		+= modules/SiKUploader
#MODULES		+= modules/bt_cfg
#MODULES		+= modules/cl_helper
#MODULES		+= modules/fs_test
#MODULES		+= modules/kbd_test
#MODULES		+= modules/sensors_probe
#MODULES		+= modules/sensors_switch
#MODULES		+= modules/serial_echo
#MODULES		+= modules/serial_measure_latency
#MODULES		+= modules/spi_exchange
#MODULES		+= modules/stty
#MODULES		+= systemcmds/bl_update
#MODULES		+= systemcmds/boardinfo
#MODULES		+= systemcmds/esc_calib
#MODULES		+= systemcmds/tests
#MODULES		+= systemcmds/writefile

#
# General system control
#
MODULES		+= modules/commander
MODULES		+= modules/gpio_led
MODULES		+= modules/mavlink
MODULES		+= modules/mobile
MODULES		+= modules/navigator
#MODULES		+= modules/uavcan

#
# Estimation modules (EKF/ SO3 / other filters)
#
MODULES		+= modules/attitude_estimator_q
MODULES		+= modules/position_estimator_inav
#MODULES		+= examples/flow_position_estimator
#MODULES		+= modules/airdog/trajectory_calculator
#MODULES		+= modules/attitude_estimator_ekf
#MODULES		+= modules/attitude_estimator_so3
#MODULES		+= modules/ekf_att_pos_estimator

#
# Vehicle Control
#
MODULES		+= modules/mc_att_control
MODULES		+= modules/mc_pos_control

#
# Logging
#
MODULES		+= modules/quick_log
MODULES		+= modules/sdlog2

#
# Unit tests
#
#MODULES 	+= modules/commander/commander_tests
#MODULES 	+= modules/unit_test

#
# Library modules
#
MODULES		+= modules/controllib
MODULES		+= modules/dataman
MODULES		+= modules/systemlib
MODULES		+= modules/systemlib/mixer
MODULES		+= modules/uORB

#
# Libraries
#
LIBRARIES	+= lib/mathlib/CMSIS
MODULES		+= lib/activity
MODULES		+= lib/airdog/hwinfo
MODULES		+= lib/conversion
MODULES		+= lib/geo
MODULES		+= lib/geo_lookup
MODULES		+= lib/mathlib
MODULES		+= lib/mathlib/math/filter
MODULES		+= lib/stm32f4
#MODULES		+= lib/ecl
#MODULES		+= lib/external_lgpl
#MODULES		+= lib/launchdetection

#
# Transitional support - add commands from the NuttX export archive.
#
# In general, these should move to modules over time.
#
# Each entry here is <command>.<priority>.<stacksize>.<entrypoint> but we use a helper macro
# to make the table a bit more readable.
#
define _B
	$(strip $1).$(or $(strip $2),SCHED_PRIORITY_DEFAULT).$(or $(strip $3),CONFIG_PTHREAD_STACK_DEFAULT).$(strip $4)
endef

#                  command                 priority                   stack  entrypoint
BUILTIN_COMMANDS := \
	$(call _B, sercon,                 ,                          2048,  sercon_main                ) \
	$(call _B, serdis,                 ,                          2048,  serdis_main                )
