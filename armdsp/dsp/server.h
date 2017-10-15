#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdbool.h>

typedef struct _Server Server; 

Server * server_new();
bool     server_start(Server *server);

#endif
