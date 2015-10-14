MODULE_COMMAND = sensor_validation
SRCS = sensor_validation.cpp \
		sensor_validation_params.c

MODULE_STACKSIZE = 1200

DEFAULT_VISIBILITY = protected 

CXXFLAGS += -std=gnu++11