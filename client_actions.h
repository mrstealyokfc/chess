#ifndef __CLIENT_ACTIONS_H
#define __CLIENT_ACTIONS_H

#include<poll.h>

void process_client_event(struct pollfd* client, char* msg);

#endif // __CLIENT_ACTIONS_H
