#ifndef MSGS_H
#define MSGS_H

#include "buffers.h"

#define MAX_MSG_IN_BUFFER 2 // maximum number of messages in buffer

typedef enum {
    START1, // waiting for first flag
    START2, // waiting for second flag       
    SIZE, // checking size
    DATA, // getting data
    END1,
    END2,
    ERROR, // error
} State;

void processMsg(volatile Buffer *raw_buffer);

#endif // MSGS_H
