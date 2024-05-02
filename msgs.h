#ifndef MSGS_H
#define MSGS_H

#include "buffers.h"

#define MAX_MSG_IN_BUFFER 2 // maximum number of messages in buffer


void processMsg(volatile Buffer *raw_buffer);

#endif // MSGS_H
