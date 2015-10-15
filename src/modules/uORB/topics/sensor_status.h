#pragma once

#include "../uORB.h"

/*
 * Sensor status codes from the most fatal to the least fatal.
 * Negative status indicates that sensor needs to be rechecked.
 * WARNING! Check that sdlog types define bellow matches the underlying type of the enum if you add new values!
 */
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
#define SENSOR_STATUS_SDLOG_TYPES "bbbbbb" // Might change with additional items in the enum!
#define SENSOR_STATUS_SDLOG_NAMES "All,Acc,Att,Baro,Gyro,Mag"

inline void set_all_sensor_statuses(struct sensor_status_s *sensor_status, const SENSOR_STATUS to_set) {
	sensor_status->combined_status = to_set;
	sensor_status->accel_status = to_set;
	sensor_status->attitude_status = to_set;
	sensor_status->baro_status = to_set;
	sensor_status->gyro_status = to_set;
	sensor_status->mag_status = to_set;
}

ORB_DECLARE(sensor_status);
