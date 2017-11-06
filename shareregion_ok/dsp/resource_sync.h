#ifndef _RESOURCE_SYNC_H_
#define _RESOURCE_SYNC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ResourceSync ResourceSync;

// @event_id is used for send event by ipc notify module, so it must be specialized
ResourceSync *resource_sync_new(uint16_t proc_id, uint16_t line_id, uint32_t event_id, uint32_t sync_id);  
void          resource_sync_destroy(ResourceSync *sync);

bool          resource_sync_pair_wait(ResourceSync *sync);   // resource sync object was ready
bool          resource_sync_pair_post(ResourceSync *sync);   // wait for another resource sync object had ready

bool          resource_sync_wait(ResourceSync *sync, uint32_t resource_id);  // wait resource done
bool          resource_sync_post(ResourceSync *sync, uint32_t resource_id);  // rsource had done

#ifdef __cplusplus
}
#endif

#endif
