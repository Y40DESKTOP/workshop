#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define NS_SHARED_TABLE_NAME    "shared_table"
#define NS_KEY_NAME_BUFFER_ADDR "buffer_addr"

typedef enum {
    RESOURCE_SYNC_ID_SYNC      = 0,

    RESOURCE_SYNC_ID_SIGNAL_START, 
    RESOURCE_SYNC_ID_SIGNAL_FINISH, 
} ResourceID; 

static inline const char *resoruce_id2str(int id) {
    switch(id) {
    case RESOURCE_SYNC_ID_SIGNAL_START:
        return "\"SIGNAL_START\"";
    case RESOURCE_SYNC_ID_SIGNAL_FINISH:
        return "\"SIGNAL_FINISH\"";
    default:
        return "\"UNKNOW\"";
    }
}

#endif
