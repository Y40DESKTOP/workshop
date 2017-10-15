#ifndef _SYSLINK_SERVICE_H_
#define _SYSLINK_SERVICE_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct _SyslinkService SyslinkService; 
typedef void (* SyslinkServiceFunc)(void *user_data);

SyslinkService * syslink_service_new(const char *host);
void             syslink_service_destroy(SyslinkService *service);

uint16_t        syslink_service_host_id(SyslinkService *service);

void            syslink_service_install(SyslinkService *service, SyslinkServiceFunc func, void *user_data);

// ipc connect to host and run installed service function. do not use ipc or syslink module before this start service function called
bool            syslink_service_start(SyslinkService *service);    

#endif
