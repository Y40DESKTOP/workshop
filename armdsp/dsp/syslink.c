#include <xdc/std.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/knl/Task.h>

#include <ti/ipc/Ipc.h>
#include <ti/ipc/MultiProc.h>

#include <ti/syslink/SysLink.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "syslink.h"

#define HOST_LEN_MAX 20

struct _Syslink {
    char host[HOST_LEN_MAX];
    int status;
    uint16_t host_id;
};

static bool syslink_is_valid(Syslink *syslink);

Syslink *syslink_new(const char *host) {
    Syslink *syslink = (Syslink *)calloc(1, sizeof(Syslink));
    syslink->status = 0;
    strncpy(syslink->host, host, HOST_LEN_MAX - 1);
    syslink->host_id = MultiProc_getId((String)host);

    do {
        LOG_TRACE("syslink try to start");
        syslink->status = Ipc_start();
    } while (syslink->status == Ipc_E_NOTREADY);
    if (syslink->status < 0) 
        LOG_ERROR("Ipc_start error=%d", (IArg)syslink->status);

    LOG_DEBUG("initialize SysLink");
    SysLink_setup();    // must be after Ipc_start()

    return syslink;
}

void syslink_destroy(Syslink *syslink) {
    if (! syslink)
        return;
        
    SysLink_destroy();  // must be before Ipc_stop()
    Ipc_stop();
    free(syslink);
}

bool syslink_connnect(Syslink *syslink) {
    ASSERT(syslink);

    if (! syslink_is_valid(syslink)) {
        LOG_ERROR("connect operation stop by previous error");
        return false;
    }

    do {
        syslink->status = Ipc_attach(syslink->host_id);
        if (syslink->status == Ipc_E_NOTREADY) 
            Task_sleep(1);
    } while (syslink->status == Ipc_E_NOTREADY);

    if (syslink->status < 0) {
        LOG_ERROR("Ipc_attach error=%d", (IArg)syslink->status);
        return false;
    }

    LOG_DEBUG("syslink connected to %s", syslink->host);
    return true;
}

bool syslink_disconnect(Syslink *syslink) {
    ASSERT(syslink);

    if (! Ipc_isAttached(syslink->host_id)) {
        LOG_WARN("there is no connection with %s", syslink->host);
        return false;
    }

    do {
        syslink->status = Ipc_detach(syslink->host_id);
        if (syslink->status == Ipc_E_NOTREADY) 
            Task_sleep(1);
    } while (syslink->status == Ipc_E_NOTREADY);

    if (syslink->status < 0) {
        LOG_ERROR("Ipc_detach error=%d", (IArg)syslink->status);
        return false;
    }

    LOG_DEBUG("syslink disconnected with %s ", syslink->host);

    return true;
}

bool syslink_is_valid(Syslink *syslink) {
    ASSERT(syslink);

    return syslink->status == Ipc_S_SUCCESS;
}

uint16_t syslink_host_id(Syslink *syslink) {
    ASSERT(syslink);
    return syslink->host_id;
}
