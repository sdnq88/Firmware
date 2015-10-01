#pragma once

#include <drivers/drv_tone_alarm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *TONE_ALARM_HANDLE;

__EXPORT TONE_ALARM_HANDLE tone_alarm_init();
__EXPORT int tone_alarm_play(TONE_ALARM_HANDLE handle, int tune);
__EXPORT void tone_alarm_close(TONE_ALARM_HANDLE handle);

#ifdef __cplusplus
}
#endif
