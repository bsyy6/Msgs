#ifndef MSGS_H
#define MSGS_H

#include "buffers.h"

// message structure => [firstStartFlag,seocondStartFlag,Size,{DATA[]},firstEndFlag,firstStartFlag]
#define firstStartFlag 0xA1
#define secondStartFlag 0xA2
#define firstEndFlag 0xA2
#define secondEndFlag 0xA1

#define NUM_OF_FLAGS 4 //  2 start flags + 2 end flags
#define NUM_OF_EXTRA_BYTES 1 // 1 byte for size
#define MIN_MSG_SIZE (NUM_OF_FLAGS+NUM_OF_EXTRA_BYTES+1)
#define MAX_MSG_SIZE 15 // maximum expected single message size
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

typedef struct {
    // any time a valid message is detected inside the buffer these values are updated.
    volatile Buffer *msgQueue; // handles the biggest message possible two times
    volatile Buffer *rawBuffer;
    uint8_t msgSize;
    uint8_t msgArrived;
    uint8_t startSearch;
    // private variables
    State   state;
    State   prevState;
    uint8_t byte;
    uint8_t prevByte;
    uint8_t size;
    uint8_t msgCount;
}msg;



msg initMsg( volatile Buffer *msgs_buffer, volatile Buffer *raw_buffer);
void processMsg(msg *m);

#endif // MSGS_H
