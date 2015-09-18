#pragma once

#include "../uORB.h"

// Sensor status codes from the most fatal to the least fatal. Negative status indicates that sensor needs to be rechecked
typedef enum {
	SENSOR_STATUS_INSUFFICIENT = -5, // Insufficient valid data collected - rather fatal. Either our configuration is wrong, or sensor is not working properly
	SENSOR_STATUS_UNSTABLE = -4, // Samples variate too much - possibly the device was moved during the test
	SENSOR_STATUS_WRONG = -3, // Sampled value differs from expected - possibly device was positioned incorrectly
	SENSOR_STATUS_FAIL = -2, // General error
	SENSOR_STATUS_CHECK_NEEDED = -1, // Conditions require us to redo the sensor check
	SENSOR_STATUS_UNKNOWN = 0, // Default state doesn't indicate that a check is required. We might be waiting for GPS to compare to last check date
	SENSOR_STATUS_OK = 1 // All is fine with the world
} SENSOR_STATUS;

struct sensor_status_s {
	SENSOR_STATUS combined_status; // Should never be better (greater) than any specific status
	SENSOR_STATUS accel_status;
	SENSOR_STATUS attitude_status;
	SENSOR_STATUS baro_status;
	SENSOR_STATUS gyro_status;
	SENSOR_STATUS mag_status;
};

ORB_DECLARE(sensor_status);
