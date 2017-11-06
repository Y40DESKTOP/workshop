#include <stdlib.h>
#include <string.h>

#include <ti/syslink/Std.h>     
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/SysLink.h>
#include <ti/ipc/MultiProc.h>

#include "log.h"
#include "syslink.h"

struct _Syslink {
    char *dsp;
    int status;
    uint16_t slave_id;
};

Syslink *syslink_new(const char *dsp) {
    Syslink *syslink = (Syslink *)calloc(1, sizeof(Syslink));
    syslink->dsp = strdup(dsp);
    SysLink_setup();
    syslink->slave_id = MultiProc_getId(syslink->dsp);

    return syslink;
}

void syslink_destroy(Syslink *syslink) {
    if (! syslink)
        return;

    SysLink_destroy();
    free(syslink->dsp);
    free(syslink);
}

bool syslink_start(Syslink *syslink) {
    ASSERT(syslink);

    if (Ipc_control(syslink->slave_id, Ipc_CONTROLCMD_LOADCALLBACK, NULL) < 0) {
        LOG_ERROR("load callback failed");
        return false;
    }

    if (Ipc_control(syslink->slave_id, Ipc_CONTROLCMD_STARTCALLBACK, NULL) < 0) {
        LOG_ERROR("start callback failed");
        return false;
    }

    return true;
}

bool syslink_stop(Syslink *syslink) {
    ASSERT(syslink);
    if (Ipc_control(syslink->slave_id, Ipc_CONTROLCMD_STOPCALLBACK, NULL) < 0) {
        LOG_ERROR("stop callback failed");
        return false;
    }

    return true;
}

uint16_t syslink_slave_id(Syslink *syslink) {
    ASSERT(syslink);
    return syslink->slave_id;
}

