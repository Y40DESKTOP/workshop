#ifndef _Syslink_H_
#define _Syslink_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct _Syslink Syslink;

Syslink *syslink_new(const char *host); 
void     syslink_destroy(Syslink *syslink);

bool     syslink_connnect(Syslink *syslink); 
bool     syslink_disconnect(Syslink *syslink);

uint16_t syslink_host_id(Syslink *syslink);


#endif
