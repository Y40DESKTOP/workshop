#ifndef _SYSLINK_H_
#define _SYSLINK_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Syslink Syslink;

Syslink * syslink_new(const char *dsp);
void      syslink_destroy(Syslink *syslink);

bool      syslink_start(Syslink *syslink);
bool      syslink_stop(Syslink *syslink);

uint16_t  syslink_slave_id(Syslink *syslink);

#ifdef __cplusplus
}
#endif

#endif
