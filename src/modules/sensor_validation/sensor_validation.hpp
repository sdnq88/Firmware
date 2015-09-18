#pragma once

/*
 * Requests sensor check to be made and sensor_status updated
 * Returns false if sensor validation thread is not running or check is already in progress
 */
extern "C" __EXPORT bool request_sensor_check();
