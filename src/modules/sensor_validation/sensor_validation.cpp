#include <nuttx/config.h>

#include <math.h>
#include <poll.h>
#include <stm32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <dog_debug.hpp>
#include <drivers/drv_hrt.h>
#include <quick_log/quick_log.hpp>
#include <statistics/statistics.hpp>
#include <systemlib/err.h>
#include <systemlib/param/param.h>
#include <systemlib/systemlib.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/sensor_status.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_gps_position.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/uORB.h>


static bool g_thread_should_run = false;
static bool g_thread_running = false;
static bool g_intensive_check = false;

constexpr uint64_t lazy_wait_interval_us = 1 * 1000 * 1000;
constexpr uint32_t sampling_interval_ms = 0; // 0 - unlimited
constexpr uint32_t poll_timeout_ms = 100; // don't push too hard
constexpr uint64_t hours_to_usec = (uint64_t) 60 * 60 * 1000 * 1000;
constexpr uint64_t gpos_fresh_timeout = 2 * 1000 * 1000;

using statistics::Variance;

bool is_recheck_required();
void validate_attitude(const Variance &pitch_variance, const Variance &roll_variance,
		const Variance &yaw_variance, const uint32_t sample_cycle_count,
		sensor_status_s &sens_status, SENSOR_STATUS &res);
void update_validation_conditions();

extern "C" __EXPORT bool request_sensor_check() {
	if (g_thread_running && !g_intensive_check) {
		g_intensive_check = true;
		return true;
	}
	return false;
}

int thread_main(int argc, char* argv[]) {

	int vstatus_sub = orb_subscribe(ORB_ID(vehicle_status));
	vehicle_status_s vstatus;
	int attitude_sub = orb_subscribe(ORB_ID(vehicle_attitude));
	vehicle_attitude_s vattitude;
	orb_set_interval(attitude_sub, sampling_interval_ms);
	uint32_t sample_cycle_count = 0;

	uint32_t tmp_int_param;
	param_get(param_find("SVAL_CHK_TIME_US"), &tmp_int_param);
	hrt_abstime check_time_us = tmp_int_param;

	Variance pitch_variance;
	Variance roll_variance;
	Variance yaw_variance;

	pollfd poller;
	poller.fd = attitude_sub;
	poller.events = POLLIN;
	int poll_ret;

	hrt_abstime intensive_check_start = 0;
	SENSOR_STATUS res = SENSOR_STATUS_OK;

	int sens_status_pub;
	sensor_status_s sens_status;
	memset(&sens_status, 0, sizeof(sens_status));
	// Publish "unknown status" before we are able to check the date
	sens_status_pub = orb_advertise(ORB_ID(sensor_status), &sens_status);

	warnx("Thread started.");
	g_thread_running = true;

	if (is_recheck_required()) {
		DOG_PRINT("Sensor validation required.\n");
		sens_status.combined_status = SENSOR_STATUS_CHECK_NEEDED;
		sens_status.accel_status = SENSOR_STATUS_CHECK_NEEDED;
		sens_status.attitude_status = SENSOR_STATUS_CHECK_NEEDED;
		sens_status.baro_status = SENSOR_STATUS_CHECK_NEEDED;
		sens_status.gyro_status = SENSOR_STATUS_CHECK_NEEDED;
		sens_status.mag_status = SENSOR_STATUS_CHECK_NEEDED;
	}
	else {
		DOG_PRINT("Sensor validation is not necessary.\n");
		sens_status.combined_status = SENSOR_STATUS_OK;
		sens_status.accel_status = SENSOR_STATUS_OK;
		sens_status.attitude_status = SENSOR_STATUS_OK;
		sens_status.baro_status = SENSOR_STATUS_OK;
		sens_status.gyro_status = SENSOR_STATUS_OK;
		sens_status.mag_status = SENSOR_STATUS_OK;
	}
	// Publish sensor_status only if we weren't stopped in the middle of the check
	if (g_thread_should_run) {
		orb_publish(ORB_ID(sensor_status), sens_status_pub, &sens_status);
	}

	while (g_thread_should_run) {

		orb_copy(ORB_ID(vehicle_status), vstatus_sub, &vstatus);
		if (vstatus.arming_state == ARMING_STATE_ARMED || vstatus.arming_state == ARMING_STATE_ARMED_ERROR) {
			usleep(lazy_wait_interval_us); // No use to poll - vstatus updates much more frequently
			continue; // Don't do anything while we are armed
		}

		if (g_intensive_check) {
			if (intensive_check_start == 0) {
				DOG_PRINT("Intensive check started.\n");
				intensive_check_start = hrt_absolute_time();
				sample_cycle_count = 0;
				pitch_variance.reset();
				roll_variance.reset();
				yaw_variance.reset();
				// Reset all statuses to UNKNOWN while the check is in progress
				memset(&sens_status, 0, sizeof(sens_status));
				orb_publish(ORB_ID(sensor_status), sens_status_pub, &sens_status);
			}
			sample_cycle_count++;

			// TODO! [AK] Process other sensors
			poll_ret = poll(&poller, 1, sampling_interval_ms + poll_timeout_ms);
			if (poll_ret == 1) {
				orb_copy(ORB_ID(vehicle_attitude), attitude_sub, &vattitude);
				if (isfinite(vattitude.pitch)) {
					pitch_variance.sample(vattitude.pitch);
				}
				if (isfinite(vattitude.roll)) {
					roll_variance.sample(vattitude.roll);
				}
				if (isfinite(vattitude.yaw)) {
					yaw_variance.sample(vattitude.yaw);
				}
			}
			else {
				// TODO! [AK] Consider better error handling
				warn("Sens_validation: poll error or timeout!");
			}

			// Stop early in case of a serious error
			if (res != SENSOR_STATUS_OK) {
				sens_status.combined_status = res;
				// Assume that sensor-specific statuses are updated
				orb_publish(ORB_ID(sensor_status), sens_status_pub, &sens_status);

				QLOG_literal("Sens_validation: Failed during the sampling!");

				res = SENSOR_STATUS_OK;
				g_intensive_check = false;
				intensive_check_start = 0;

				continue;
			}

			if (hrt_absolute_time() - intensive_check_start > check_time_us) {
				validate_attitude(pitch_variance, roll_variance, yaw_variance, sample_cycle_count, sens_status, res);

				sens_status.combined_status = res;
				// Assume that sensor-specific statuses are updated
				orb_publish(ORB_ID(sensor_status), sens_status_pub, &sens_status);

				if (res == SENSOR_STATUS_OK) {
					// Update date and temperature in case of success
					update_validation_conditions();
				}

				DOG_PRINT("Sens_validation: Finished intensive check! Res is %d.\n", res);

				g_intensive_check = false;
				intensive_check_start = 0;
				res = SENSOR_STATUS_OK;
			}
		}
		else { // Do minimal sanity checks periodically
			/* TODO! [AK] Not implemented yet.
			 * Should set sensor status to invalid in critical cases
			 * We can get out of the invalid status only through intensive check
			 */
			usleep(lazy_wait_interval_us);
		}

	}
	orb_unsubscribe(vstatus_sub);
	orb_unsubscribe(attitude_sub);
	orb_unsubscribe(sens_status_pub);

	g_thread_running = false;
	g_thread_should_run = false; // In case we left because of an error
	g_intensive_check = false; // Don't do an intensive check in case we were stopped before we could finish
	return 0;
}

bool is_recheck_required() {
	uint32_t tmp_int_param;
	param_get(param_find("SVAL_LAST_DATE"), &tmp_int_param);
	uint64_t last_check_time = tmp_int_param * hours_to_usec;
	param_get(param_find("SVAL_HOUR_THRESH"), &tmp_int_param);
	uint64_t time_threshold = tmp_int_param * hours_to_usec;
	float last_check_temp;
	param_get(param_find("SVAL_LAST_TEMP"), &last_check_temp);
	float temp_threshold;
	param_get(param_find("SVAL_TEMP_THRESH"), &temp_threshold);
	if (last_check_time == 0 || time_threshold == 0 || temp_threshold <= 0.0f) {
		return true;
	}

	// Wait for inav to consider GPS valid before the check
	int gpos_sub = orb_subscribe(ORB_ID(vehicle_global_position));
	vehicle_global_position_s gpos_data;
	bool updated = false;
	while ((!updated || (hrt_absolute_time() - gpos_data.timestamp) > gpos_fresh_timeout) && g_thread_should_run) {
		/*
		 * Use sleep to periodically check if thread should stop. Initial sleep is ok as gpos, probably, is not ready
		 */
		DOG_PRINT("Waiting for GPOS\n");
		sleep(2);
		orb_check(gpos_sub, &updated);
		if (updated) {
			orb_copy(ORB_ID(vehicle_global_position), gpos_sub, &gpos_data);
		}
	}
	orb_unsubscribe(gpos_sub);

	// Now actual GPS check
	int gps_sub = orb_subscribe(ORB_ID(vehicle_gps_position));
	vehicle_gps_position_s gps_data;
	bool gps_ok = false;
	updated = (orb_copy(ORB_ID(vehicle_gps_position), gps_sub, &gps_data)) == 0;

	while ((!updated || gps_data.timestamp_time == 0
			|| gps_data.timestamp_variance == 0 || gps_data.fix_type < 2)
			&& g_thread_should_run) {
		/*
		 * If GPS data is bad, it won't change as fast as topic update rate, so just sleep between checks
		 */
		DOG_PRINT("Waiting for the GPS\n");
		sleep(2);
		orb_check(gps_sub, &updated);
		if (updated) {
			orb_copy(ORB_ID(vehicle_gps_position), gps_sub, &gps_data);
		}
	}
	orb_unsubscribe(gps_sub);
	if (updated && gps_data.timestamp_time != 0) {
		DOG_PRINT("GPS time: %lld\n", gps_data.time_gps_usec);
	}
	// If time ran backwards or there there is no GPS lock, then GPS is invalid by default
	if (updated && gps_data.timestamp_time != 0 && gps_data.time_gps_usec >= last_check_time
			&& gps_data.timestamp_variance != 0 && gps_data.fix_type >= 2) {
		if (gps_data.time_gps_usec <= last_check_time + time_threshold) {
			gps_ok = true;
		}
	}

	if (!gps_ok){
		return true;
	}

	// Poll temperature second to allow the board to heat up while waiting for GPS
	int sens_comb_sub = orb_subscribe(ORB_ID(sensor_combined));
	sensor_combined_s sens_comb;
	uint32_t retry_count = 10;
	pollfd poller;
	poller.fd = sens_comb_sub;
	poller.events = POLLIN;
	int poll_ret;

	while (retry_count > 0) {
		retry_count--;
		poll_ret = poll(&poller, 1, 1000);
		if (poll_ret == 1) {
			orb_copy(ORB_ID(sensor_combined), sens_comb_sub, &sens_comb);
			if (sens_comb.accelerometer_timestamp != 0) {
				DOG_PRINT("Accel temp: %.6f\n", (double) sens_comb.accelerometer_temperature);
				break;
			}
		}
		else {
			warn("Sens_validation: poll error or timeout!");
		}
	}
	orb_unsubscribe(sens_comb_sub);
	if (retry_count == 0 || fabsf(sens_comb.accelerometer_temperature - last_check_temp) > temp_threshold) {
		// Panic in case we failed to get temperature in 10 seconds
		return true;
	}
	else {
		return false;
	}
}

void update_validation_conditions() {
	int sens_comb_sub = orb_subscribe(ORB_ID(sensor_combined));
	sensor_combined_s sens_comb;
	int res;
	bool params_need_saving = false;
	res = orb_copy(ORB_ID(sensor_combined), sens_comb_sub, &sens_comb);
	if (res == 0 && sens_comb.accelerometer_timestamp > 0) {
		param_set(param_find("SVAL_LAST_TEMP"), &sens_comb.accelerometer_temperature);
		params_need_saving = true;
	}
	orb_unsubscribe(sens_comb_sub);

	int gps_sub = orb_subscribe(ORB_ID(vehicle_gps_position));
	vehicle_gps_position_s gps_data;
	res = orb_copy(ORB_ID(vehicle_gps_position), gps_sub, &gps_data);
	if (res == 0 && gps_data.timestamp_time > 0
			&& gps_data.timestamp_variance != 0 && gps_data.fix_type >= 2) {
		uint32_t time_hours = gps_data.time_gps_usec / hours_to_usec;
		param_set(param_find("SVAL_LAST_DATE"), &time_hours);
		params_need_saving = true;
	}
	orb_unsubscribe(gps_sub);
	if (params_need_saving) {
		param_save_default();
	}
}

void validate_attitude(const Variance &pitch_variance, const Variance &roll_variance,
		const Variance &yaw_variance, const uint32_t sample_cycle_count,
		sensor_status_s &sens_status, SENSOR_STATUS &res) {

	float tmp_float_param;
	param_get(param_find("SVAL_ATT_THRESH"), &tmp_float_param);
	float attitude_variance_threshold = tmp_float_param * M_DEG_TO_RAD_F;
	attitude_variance_threshold *= attitude_variance_threshold;
	param_get(param_find("SVAL_ATT_ANGLE"), &tmp_float_param);
	float attitude_mean_threshold = tmp_float_param * M_DEG_TO_RAD_F;
	attitude_mean_threshold *= attitude_mean_threshold;
	float success_fraction;
	param_get(param_find("SVAL_GOOD_FRACT"), &success_fraction);

	if (pitch_variance.get_dataset_size() < sample_cycle_count * success_fraction
			|| roll_variance.get_dataset_size() < sample_cycle_count * success_fraction
			|| yaw_variance.get_dataset_size() < sample_cycle_count * success_fraction) {
		if (res > SENSOR_STATUS_INSUFFICIENT) {
			res = SENSOR_STATUS_INSUFFICIENT;
		}
		sens_status.attitude_status = SENSOR_STATUS_INSUFFICIENT;
		QLOG_literal("Sens_validation: Attitude validation insufficient samples!");
		DOG_PRINT("Expected sample count: %6.1f, pitch: %d,\n\troll: %d, yaw: %d\n",
				(double) (sample_cycle_count * success_fraction), pitch_variance.get_dataset_size(),
				roll_variance.get_dataset_size(), yaw_variance.get_dataset_size());
	}
	else if (pitch_variance.get_variance() > attitude_variance_threshold
			|| roll_variance.get_variance() > attitude_variance_threshold
			|| yaw_variance.get_variance() > attitude_variance_threshold) {
		if (res > SENSOR_STATUS_UNSTABLE) {
			res = SENSOR_STATUS_UNSTABLE;
		}
		sens_status.attitude_status = SENSOR_STATUS_UNSTABLE;
		warnx("Attitude variance is too high.");
		DOG_PRINT("Expected variance: %.6f, pitch: %.6f,\n\troll: %.6f, yaw: %.6f\n",
				(double) attitude_variance_threshold, (double) pitch_variance.get_variance(),
				(double) roll_variance.get_variance(), (double) yaw_variance.get_variance());
	}
	else if (powf(fabsf(pitch_variance.get_mean()), 2)
			+ powf(fabsf(roll_variance.get_mean()), 2) > attitude_mean_threshold) {
		if (res > SENSOR_STATUS_WRONG) {
			res = SENSOR_STATUS_WRONG;
		}
		sens_status.attitude_status = SENSOR_STATUS_WRONG;
		warnx("Attitude deviation is too high.");
		DOG_PRINT("Expected deviation: %.6f, pitch: %.6f,\n\troll: %.6f square sum: %.6f\n",
				(double) attitude_mean_threshold, (double) pitch_variance.get_mean(),
				(double) roll_variance.get_mean(),
				(double) (powf(fabsf(pitch_variance.get_mean()), 2) + powf(fabsf(roll_variance.get_mean()), 2)));
	}
	else {
		sens_status.attitude_status = SENSOR_STATUS_OK;
		DOG_PRINT("Expected variance: %.6f, pitch: %.6f,\n\troll: %.6f, yaw: %.6f\n",
				(double) attitude_variance_threshold, (double) pitch_variance.get_variance(),
				(double) roll_variance.get_variance(), (double) yaw_variance.get_variance());
		DOG_PRINT("Expected deviation: %.6f, pitch: %.6f,\n\troll: %.6f square sum: %.6f\n",
				(double) attitude_mean_threshold, (double) pitch_variance.get_mean(),
				(double) roll_variance.get_mean(),
				(double) (powf(fabsf(pitch_variance.get_mean()), 2) + powf(fabsf(roll_variance.get_mean()), 2)));
	}
}

extern "C" __EXPORT int sensor_validation_main(int argc, char* argv[]) {

	if (argc == 2 && strcmp(argv[1], "start") == 0) {
		int enabled = 1;
		param_get(param_find("SVAL_ENABLE"), &enabled);
		if (enabled == 0) {
			sensor_status_s sens_status;
			sens_status.combined_status = SENSOR_STATUS_OK;
			sens_status.accel_status = SENSOR_STATUS_OK;
			sens_status.attitude_status = SENSOR_STATUS_OK;
			sens_status.baro_status = SENSOR_STATUS_OK;
			sens_status.gyro_status = SENSOR_STATUS_OK;
			sens_status.mag_status = SENSOR_STATUS_OK;
			int sens_status_pub = orb_advertise(ORB_ID(sensor_status), &sens_status);
			close(sens_status_pub);
			return 0;
		}
		if (!g_thread_running && !g_thread_should_run) {
			g_thread_should_run = true;
			task_spawn_cmd("sens_validation_daemon",
				SCHED_DEFAULT,SCHED_PRIORITY_DEFAULT, 2500,
				thread_main,
				(const char**) NULL);
		}
		else {
			warnx("Thread either scheduled to run or is already running.");
		}
	}
	else if (argc == 2 && strcmp(argv[1], "stop") == 0) {
		g_thread_should_run = false;
		usleep(lazy_wait_interval_us + 100000);
		if (g_thread_running) {
			usleep(lazy_wait_interval_us + 100000);
		}
		if (!g_thread_running) {
			warnx("Stopped.");
		}
		else {
			warnx("Failed!");
		}
	}
	else if (argc == 2 && strcmp(argv[1], "status") == 0) {
		if (g_thread_running) {
			warnx("Running");
		}
		else {
			warnx("Not running");
		}
	}
	else if (argc == 2 && strcmp(argv[1], "check") == 0) {
		if (!request_sensor_check()) {
			warnx("Sensor check already in progress or main thread is not running!");
		}
	}
	else {
		printf("Usage: sensor_validation <start|stop|status|check>\n");
	}

	return 0;
}
