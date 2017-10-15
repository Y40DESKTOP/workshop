/*
 * Copyright (C) Guang Zhou Chuang Long Electronic Technology Co., Ltd - http://www.tronlong.com
 */

#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#include <ti/syslink/Std.h>     
#include <ti/ipc/Notify.h>

#include "resource_sync.h"
#include "log.h"

struct _ResourceSync {
    uint16_t proc_id;
    uint16_t line_id;
    uint32_t event_id;

    uint32_t sync_id;
    uint32_t ready_id;

    sem_t sem;
};

static void resource_sync_event_handle(uint16_t proc_id, uint16_t line_id, uint32_t event_id, UArg arg, uint32_t ready_id);

ResourceSync *resource_sync_new(uint16_t proc_id, uint16_t line_id, uint32_t event_id, uint32_t sync_id) {
    ResourceSync *sync = (ResourceSync *)calloc(1, sizeof(ResourceSync));
    sync->proc_id = proc_id;
    sync->line_id = line_id;
    sync->event_id = event_id;
    sync->sync_id = sync_id;

    if (sem_init(&sync->sem, 0, 0) < 0)
        LOG_ERROR("fail to initialize semphore");

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

    sem_post(&sync->sem);
}
    
void resource_sync_destroy(ResourceSync *sync) {
    if (! sync)
        return;

    Notify_unregisterEvent(sync->proc_id, sync->line_id, sync->event_id, resource_sync_event_handle, (UArg)sync);
    sem_destroy(&sync->sem);
    free(sync);
}

bool resource_sync_pair_wait(ResourceSync *sync) {
    ASSERT(sync);

    int status;
    do {
        LOG_TRACE("try sync");
        status = Notify_sendEvent(sync->proc_id, sync->line_id, sync->event_id, sync->sync_id, TRUE);
        if (status == Notify_E_EVTNOTREGISTERED) {
            usleep(100);
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
        sem_wait(&sync->sem);

    return true; 
}

bool resource_sync_post(ResourceSync *sync, uint32_t resource_id) {
    ASSERT(sync);

    LOG_DEBUG("resource %d had ready", resource_id);
    Notify_sendEvent(sync->proc_id, sync->line_id, sync->event_id, resource_id, TRUE);
    return true;
}
