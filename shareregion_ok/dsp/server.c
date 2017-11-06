#include <xdc/std.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/SharedRegion.h>

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "server.h"
#include "syslink_service.h"
#include "resource_sync.h"
#include "../shared/sys_config.h"
#include "../shared/protocol.h"

struct _Server {
    SyslinkService *service;
    ResourceSync *sync;
};

static void server_working(void *server_ptr);
static void server_destroy(Server *server);
static void server_wait_shared_buffer_ready(Server *server);
static uint8_t *server_get_shared_buffer(Server *server);
static void lower_to_upper(Server *server, uint8_t *input, uint8_t *output,uint32_t size);

Server *server_new() {
    Server *server = (Server *)calloc(1, sizeof(Server));

    server->service = syslink_service_new("HOST");

    return server;
}

void server_destroy(Server *server) {
    if (! server)
        return;

    syslink_service_destroy(server->service);
    free(server);
}

bool server_start(Server *server) {
    ASSERT(server);

    syslink_service_install(server->service, server_working, server);
    return syslink_service_start(server->service);
}

void server_working(void *server_ptr) {
    Server *server = (Server *)server_ptr;

    server->sync = resource_sync_new(syslink_service_host_id(server->service),
            SYS_CFG_LINE_ID, SYS_CFG_EVT_ID_RESOURCE_SYNC, RESOURCE_SYNC_ID_SYNC);

while(1){//while start
    // wait sync object ready
    if (! resource_sync_pair_wait(server->sync)) {
        printf("fail to be synchronize with the slave resouce sync object");
        return;
    }
    printf("synchronised sync object");

    // get shared buffer
    server_wait_shared_buffer_ready(server);
    uint8_t *buffer = server_get_shared_buffer(server);
    if (buffer == NULL) {
        printf("fail to get shared buffer");
        return;
    }
    printf("get shared buffer");

    // lower to upper
    uint32_t input_data_size = *(uint32_t *)buffer;
    printf("input data size: %d", input_data_size);
    uint8_t *input = buffer + sizeof(uint32_t);
    uint8_t *output = input + input_data_size;
    lower_to_upper(server, input, output, input_data_size);

    // post finish signal
    if (! resource_sync_post(server->sync, RESOURCE_SYNC_ID_SIGNAL_FINISH)) {
        printf("fail to be synchronize with the resource %s", resoruce_id2str(RESOURCE_SYNC_ID_SIGNAL_FINISH));
        return;
    }
    printf("synchronised resource %s", resoruce_id2str(RESOURCE_SYNC_ID_SIGNAL_FINISH));
}//while end
    resource_sync_destroy(server->sync);

    server_destroy(server);
}

static void server_wait_shared_buffer_ready(Server *server) {
    if (! resource_sync_wait(server->sync, RESOURCE_SYNC_ID_SIGNAL_START)) {
        printf("fail to be synchronize with the resource %s", resoruce_id2str(RESOURCE_SYNC_ID_SIGNAL_START));
        return;
    }
    printf("synchronised resource %s", resoruce_id2str(RESOURCE_SYNC_ID_SIGNAL_START));
}

static uint8_t *server_get_shared_buffer(Server *server) {
    // create shared table
    NameServer_Params params;
    NameServer_Params_init(&params);
    params.maxRuntimeEntries = 10;
    NameServer_Handle handle = NameServer_create(NS_SHARED_TABLE_NAME, &params);
    if (handle == NULL) {
        printf("fail to create nameserver handle");
        return NULL;
    }

    // get shared buffer addres in arm space
    uint32_t buffer_addr = 0;
    uint16_t proc_id[2] = {0, 0};
    proc_id[0] = syslink_service_host_id(server->service);
    proc_id[1] = MultiProc_INVALIDID;
    NameServer_getUInt32(handle, NS_KEY_NAME_BUFFER_ADDR, &buffer_addr, proc_id);
    printf("buffer_addr: 0x%x", buffer_addr);

    // get shared buffer address in dsp space
    SharedRegion_SRPtr shared_buffer_addr = buffer_addr;
    uint8_t *buffer = SharedRegion_getPtr(shared_buffer_addr);
    printf("buffer: 0x%x", buffer);

    // delete shared table

    NameServer_delete(&handle);

    return buffer;
}

static void lower_to_upper(Server *server, uint8_t *input, uint8_t *output, uint32_t size) {
	while(size)
	{
	    if((*input >= 65) && (*input <= 90))
		*output= *input+32;
		else
			*output= *input;
	input++;
	output++;
		size--;
	}
}

