#include <unistd.h>
#include <string.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#include "log.h"
#include "sys_config.h"
#include "protocol.h"
#include "syslink.h"
#include "resource_sync.h"

#include <ti/syslink/Std.h>     
#include <ti/syslink/utils/Memory.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/SharedRegion.h>

#define BUFFER_SIZE_4M (8 * 32 * sizeof(uint32_t))
#define BUFFER_SIZE_IN (8 * 16 * sizeof(uint32_t))

int main(int argc, char *argv[]) {

    //define the read/write files
    int fp_r;
    ssize_t ret,in_bytes = 1, out_bytes;
//    fp_r = open("/dev/davinci_mcbsp",O_RDONLY);
    fp_r = open("test.txt",O_RDONLY);
    if(fp_r < 0){
	perror("failed to open davinci_mcbsp\n");
	return -1;
    }

    int fp_w;
    fp_w = open("result.txt",O_RDWR|O_CREAT|O_TRUNC,0666);
    if(fp_w < 0){
	perror("failed to open result.txt\n");
	return -1;
    }

    // init syslink
    Syslink *syslink = syslink_new("DSP");
    syslink_start(syslink);

    // init sync object
    ResourceSync *sync = resource_sync_new(syslink_slave_id(syslink), 
            SYS_CFG_LINE_ID, SYS_CFG_EVT_ID_RESOURCE_SYNC, RESOURCE_SYNC_ID_SYNC);

while(in_bytes){

    resource_sync_pair_wait(sync);

    // calloc 4m shared buffer form shared space
    IHeap_Handle heap = (IHeap_Handle) SharedRegion_getHeap(SYS_CFG_SHARED_REGION_1);
    uint32_t *buffer = (uint32_t *)Memory_calloc(heap, BUFFER_SIZE_4M, 0, NULL);



    // write input file data and file size to shared buffer,
    // buffer[0]: input/output file size
    // buffer[1]: input file data start address
#if 1
//    QFile input_file;
//    input_file.setFileName("input.bmp");
//    input_file.open(QIODevice::ReadOnly);

    buffer[0] = (uint32_t)(BUFFER_SIZE_IN);
    in_bytes = read(fp_r,(uint8_t *)&buffer[1], buffer[0]); 
    if(!in_bytes)
    break;
    LOG_INFO("input data size: %d", in_bytes);
#endif
    // get shared buffer address in shared space
    SharedRegion_SRPtr shared_buffer_addr = SharedRegion_getSRPtr(buffer, SYS_CFG_SHARED_REGION_1);
    LOG_INFO("shared_buffer_addr: 0x%x", shared_buffer_addr);

    // shared the shared buffer address in shared space
    NameServer_Params params;
    NameServer_Params_init(&params);
    NameServer_Handle handle = NameServer_create(NS_SHARED_TABLE_NAME, &params);
    NameServer_addUInt32(handle, NS_KEY_NAME_BUFFER_ADDR, shared_buffer_addr);

    // post start signal
    resource_sync_post(sync, RESOURCE_SYNC_ID_SIGNAL_START); 
    LOG_TRACE("synchronised resource %s", resoruce_id2str(RESOURCE_SYNC_ID_SIGNAL_START));

    // wait finish signal
    resource_sync_wait(sync, RESOURCE_SYNC_ID_SIGNAL_FINISH);
    LOG_TRACE("synchronised resource %s", resoruce_id2str(RESOURCE_SYNC_ID_SIGNAL_FINISH));

    // show input bmp image
#if 1
//    QLabel label_input;
//    QPixmap pixmap_input;
//    pixmap_input.loadFromData((const uchar *)&buffer[1], (uint)buffer[0]);
    out_bytes = write(fp_w,(const uint8_t *)&buffer[1]+buffer[0], in_bytes);
    LOG_INFO("output data size: %d", out_bytes);
//    label_input.setPixmap(pixmap_input);
//    label_input.setGeometry(0, 33, 800, 207);
//    label_input.show();
#endif
    // show output bmp image
#if 0
    QLabel label_output;
    QPixmap pixmap_output;
    pixmap_output.loadFromData((const uchar *)&buffer[1] + buffer[0], (uint)buffer[0]);
    label_output.setPixmap(pixmap_output);
    label_output.setGeometry(0, 273, 800, 207);
    label_output.show();
#endif

    // realease 
    Memory_free(heap, buffer, BUFFER_SIZE_4M);
    NameServer_remove(handle, NS_KEY_NAME_BUFFER_ADDR);
    NameServer_delete(&handle);
}
    resource_sync_destroy(sync);
    syslink_stop(syslink);
    syslink_destroy(syslink);

    return 0;
}

