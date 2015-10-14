#pragma once

#ifdef __cplusplus
extern "C" {
#endif

__EXPORT int eparam_save(const char *search_string, const char *filename, const char *mode);
__EXPORT int eparam_load(const char *filename);

__EXPORT int eparam_factorySave(void);
__EXPORT int eparam_factoryLoad(void);

__EXPORT int eparam_factoryReset(void);

#ifdef __cplusplus
}
#endif
