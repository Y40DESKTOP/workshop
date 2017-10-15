#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>

#define PAYLOAD_MAX  128

typedef struct _MessagePing {
    uint32_t payload;
    uint8_t buffer[1024 * 96];
	uint8_t testb;
} MessagePing;

typedef struct _MessagePong {
    uint32_t payload;
    uint8_t buffer[1024 * 96];
	uint8_t testb;
} MessagePong;

typedef enum {
    REC_SYNC_ID_SYNC = 0,

    REC_SYNC_ID_SIG_UMSG_CREATED,
    REC_SYNC_ID_SIG_UMSG_OPENED,
    REC_SYNC_ID_SIG_UMSG_CLOSED
} ResourceID; 

#endif
