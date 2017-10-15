/*
 * Copyright (C) Guang Zhou Chuang Long Electronic Technology Co., Ltd - http://www.tronlong.com
 */

#include <stdlib.h>

#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/ipc/Notify.h>

#include "resource_sync.h"
#include "log.h"

struct _ResourceSync {
    uint16_t proc_id;
    uint16_t line_id;
    uint32_t event_id;

    uint32_t sync_id;
    uint32_t ready_id;

    Semaphore_Struct sem_obj;
    Semaphore_Handle sem; 
};

static void resource_sync_event_handle(uint16_t proc_id, uint16_t line_id, uint32_t event_id, UArg arg, uint32_t ready_id);

ResourceSync *resource_sync_new(uint16_t proc_id, uint16_t line_id, uint32_t event_id, uint32_t sync_id) {
    ResourceSync *sync = (ResourceSync *)calloc(1, sizeof(ResourceSync));
    sync->proc_id = proc_id;
    sync->line_id = line_id;
    sync->event_id = event_id;
    sync->sync_id = sync_id;

    Semaphore_Params params;
    Semaphore_Params_init(&params);
    params.mode = Semaphore_Mode_COUNTING;
    Semaphore_construct(&sync->sem_obj, 0, &params);
    sync->sem = Semaphore_handle(&sync->sem_obj);

    if (Notify_registerEvent(proc_id, line_id, event_id, resource_sync_event_handle, (UArg)sync) < 0)
        LOG_ERROR("fail to register event in %d:%d(line:event)", line_id, event_id);

    return sync;
}

static void resource_sync_event_handle(uint16_t proc_id, uint16_t line_id, uint32_t event_id, UArg arg, uint32_t ready_id) {
    (void)proc_id; (void)line_id; (void)event_id;

    ResourceSync *sync = (ResourceSync *)arg;

    LOG_DEBUG("resource %d had ready", ready_id);
    sync->ready_id = ready_id;

    if (sync->ready_id == sync->sync_id)  // send by wait pair
        return;

    Semaphore_post(sync->sem);
}
    
void resource_sync_destroy(ResourceSync *sync) {
    if (! sync)
        return;

    Notify_unregisterEvent(sync->proc_id, sync->line_id, sync->event_id, resource_sync_event_handle, (UArg)sync);
    Semaphore_destruct(&sync->sem_obj);
    free(sync);
}

bool resource_sync_pair_wait(ResourceSync *sync) {
    ASSERT(sync);

    int status;
    do {
        LOG_TRACE("try sync");
        status = Notify_sendEvent(sync->proc_id, sync->line_id, sync->event_id, sync->sync_id, TRUE);
        if (status == Notify_E_EVTNOTREGISTERED) {
            Task_sleep(100);
        }
    } while (status == Notify_E_EVTNOTREGISTERED);

    if (status < 0 ) {
        LOG_ERROR("sync faile");
        return false;
    }
    
    return true;
}

bool resource_sync_wait(ResourceSync *sync, uint32_t resource_id) {
    ASSERT(sync);

    LOG_DEBUG("wait resource %d", resource_id);
    while (sync->ready_id != resource_id)
        Semaphore_pend(sync->sem, BIOS_WAIT_FOREVER);

    return true; 
}

bool resource_sync_post(ResourceSync *sync, uint32_t resource_id) {
    ASSERT(sync);

    LOG_DEBUG("resource %d had ready", resource_id);
    Notify_sendEvent(sync->proc_id, sync->line_id, sync->event_id, resource_id, TRUE);
    return true;
}
