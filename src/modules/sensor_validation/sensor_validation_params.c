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
PARAM_DEFINE_FLOAT(SVAL_TEMP_THRESH, 5.0f);
