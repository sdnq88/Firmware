#
# Latest board revision is defined in board_AirDogFMU.mk
#

MODULES += lib/activity

MODULES += drivers/boards/AirLeash/$(CONFIG_BOARD_REVISION)
MODULES += drivers/boards/AirLeash/kbd
MODULES += modules/leash

MODULES += modules/leash_app
MODULES += modules/leash_display

ROMFS_EXTRA_FILES += $(wildcard $(PX4_BASE)ROMFS/factory/AirLeash/*)

include $(PX4_BASE)/makefiles/config_Factory_common.mk
