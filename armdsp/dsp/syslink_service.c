#include <xdc/std.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/knl/Task.h>

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "syslink.h"
#include "syslink_service.h"

struct _SyslinkService {
    Syslink *syslink;
    SyslinkServiceFunc func;
    void *user_data;
};

static void syslink_service_run(UArg service_ptr, UArg arg); 

SyslinkService *syslink_service_new(const char *host) {
    SyslinkService *service = (SyslinkService *)calloc(1, sizeof(SyslinkService));
    service->syslink = syslink_new(host);
    return service;
}

void syslink_service_destroy(SyslinkService *service) {
    if (! service)
        return;

    syslink_destroy(service->syslink);
    free(service);
}

bool syslink_service_start(SyslinkService *service) {
    ASSERT(service);
    ASSERT(service->func && "do you forget install service function");

    Error_Block eb;
    Error_init(&eb);

    Task_Params params;
    Task_Params_init(&params);
    params.instance->name = "service_run_func";
    params.arg0 = (UArg)service;
    params.arg1 = (UArg)NULL;
    params.stackSize = 0x1000;

    Task_create(syslink_service_run, &params, &eb);  // start service

    if (Error_check(&eb)) {
        LOG_ERROR("failed to create service startup thread");
        return false;
    }
    LOG_TRACE("start service ...");

    return true;
}

void syslink_service_run(UArg service_ptr, UArg arg) {
    ASSERT(service_ptr);

    SyslinkService *service = (SyslinkService *)service_ptr;

    LOG_TRACE("--> service_fun");

    if (! syslink_connnect(service->syslink))  {
        LOG_INFO("connected failed");
        goto err;
    }

    LOG_TRACE("service start ...");
    service->func(service->user_data);              // run real service function
    LOG_TRACE("service stop ...");

    if (! syslink_disconnect(service->syslink))  {
        LOG_INFO("disconnected failed");
        goto err;
    }
    
    LOG_TRACE("<-- service_fun");

err:
    syslink_service_destroy(service);

    return;
}

void syslink_service_install(SyslinkService *service, SyslinkServiceFunc func, void *user_data) {
    ASSERT(service);
    ASSERT(func);

    service->func = func;
    service->user_data = user_data;
}

uint16_t syslink_service_host_id(SyslinkService *service) {
    ASSERT(service);
    return syslink_host_id(service->syslink);
}
