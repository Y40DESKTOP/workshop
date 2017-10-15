#include "syslink.h"
#include "resource_sync.h"
#include "log.h"
#include "../shared/protocol.h"     
#include "../shared/sys_config.h"   
#include <ti/syslink/Std.h>     
#include "../shared/umsg/Umsg.h"

int main(int argc, char *argv[]) {

	//testadd
	FILE *fp_r = NULL;

LOG_DEBUG("opened success test.txt before\n");
	if((fp_r = fopen("test.txt","r+")) == NULL)
	{
		fprintf(stderr,"fail to fopen test.txt\n");
		return -1;
	}
LOG_DEBUG("opened success test.txt\n");

	FILE *fp_w = NULL;
LOG_DEBUG("opened success result.txt before\n");
	if((fp_w = fopen("result.txt","w+")) == NULL)
	{
		fprintf(stderr,"fail to fopen result.txt\n");
		return -1;
	}
LOG_DEBUG("opened success result.txt after\n");

	
    // syslink init
    Syslink * syslink = syslink_new("DSP");
    if (! syslink_connect(syslink)) {
        LOG_ERROR("fail to connect to slave");
        return -1;
    }
    LOG_DEBUG("connected to slave");

    uint32_t proc_id = syslink_proc_id(syslink);

    // create sync object //TODO: release
    ResourceSync *sync = resource_sync_new(proc_id, 
            SYS_CFG_LINE_ID, SYS_CFG_EVT_ID_RESOURCE_SYNC, REC_SYNC_ID_SYNC);
    resource_sync_pair_wait(sync);

    // umsg module init
    Umsg_setup();

    // create umsg object
    Umsg_Params params;
    Umsg_Params_init(&params);
    params.msgSize = sizeof(MessagePing); // Notice: ASSERT(msgSize % sizeof(uint32_t)) == 0)
    params.poolCount = 4;                 // Notice: ASSERT(poolCount > 3)
    params.inboxCount = 3;
    params.regionId = 0;

    Umsg_Handle umsg_ping = Umsg_create(SYS_CFG_UMSG_PING, TRUE /* writer */, proc_id, &params); 

    params.msgSize = sizeof(MessagePong); // Notice: ASSERT(msgSize % sizeof(uint32_t)) == 0)
    Umsg_Handle umsg_pong = Umsg_create(SYS_CFG_UMSG_PONG, FALSE /* reader */, proc_id, &params); 

    // ensure umsg object is ready
    resource_sync_post(sync, REC_SYNC_ID_SIG_UMSG_CREATED);  
    resource_sync_wait(sync, REC_SYNC_ID_SIG_UMSG_OPENED);  



	uint32_t payload = 0;
	uint8_t testb;
	while (payload != PAYLOAD_MAX) {        // payload up to max
		// ping
		MessagePing *ping = (MessagePing *)Umsg_alloc(umsg_ping);
		
		//bytes = read(fp,ping->testb,sizeof(ping->testb));
		ping->testb = fgetc(fp_r);
					
		ping->payload = payload;
		Umsg_put(umsg_ping, ping);  
		
		// pong
		MessagePong *pong = (MessagePong *)Umsg_get(umsg_pong);  
		payload = pong->payload;
		testb = pong->testb;
		Umsg_free(umsg_pong, pong);
		
		fputc(testb,fp_w);
		// payload
		LOG_INFO("payload: %d,%c", payload,testb);
	}
	
	fclose(fp_r);
	fclose(fp_w);
    // release umsg object
    resource_sync_wait(sync, REC_SYNC_ID_SIG_UMSG_CLOSED);    // wait for writer umsg object closed
    Umsg_delete(&umsg_ping);
    Umsg_delete(&umsg_pong);

    // release umsg module
    Umsg_destroy();

    // release sync object
    resource_sync_destroy(sync);

    // release syslink
    syslink_disconnect(syslink);
    syslink_destroy(syslink);

    LOG_DEBUG("quit ...");

    return 0;
}
