#include <nuttx/config.h>
#include <systemlib/param/param.h>

/*
 * Total time an intensive check will run in microseconds
 */
PARAM_DEFINE_INT32(SVAL_CHK_TIME_US, 15000000);
/*
 * What fraction of samples needs to be valid for calibration to succeed
 */
PARAM_DEFINE_FLOAT(SVAL_GOOD_FRACT, 0.99f);
/*
 * Variation threshold in degrees for attitude checks. Variation less than this will be considered ok
 */
PARAM_DEFINE_FLOAT(SVAL_ATT_THRESH, 0.7f);
/*
 * Variation threshold in degrees for yaw checks. Variation less than this will be considered ok
 */
PARAM_DEFINE_FLOAT(SVAL_YAW_THRESH,
#ifdef CONFIG_ARCH_BOARD_AIRLEASH
		2.1f
#else
		0.7f
#endif
);
/*
 * Threshold for mean angle (degrees) on pitch and roll together for attitude checks. Angles less than this will be ok.
 */
PARAM_DEFINE_FLOAT(SVAL_ATT_ANGLE, 5.0f);
/*
 * Hours since 1970.01.01 when last validation check was done
 */
PARAM_DEFINE_INT32(SVAL_LAST_DATE, 0);
/*
 * How many hours should pass to trigger recheck
 */
PARAM_DEFINE_INT32(SVAL_HOUR_THRESH, 8);
/*
 * Temperature at the time of the last calibration
 */
PARAM_DEFINE_FLOAT(SVAL_LAST_TEMP, -278.15f);
/*
 * What temperature difference should trigger recheck
 */
PARAM_DEFINE_FLOAT(SVAL_TEMP_THRESH,
#ifdef CONFIG_ARCH_BOARD_AIRLEASH
		12.0f
#else
		5.0f
#endif
);
/*
 * Enable or disable the validation. Disabled on 0, enabled otherwise.
 */
PARAM_DEFINE_INT32(SVAL_ENABLE, 1);
