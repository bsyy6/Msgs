#include <stdio.h>
#include "msgs.h"
#include "buffers.h"


uint8_t msgData[10];
uint8_t raw_buffer[10];

volatile Buffer rawBuffer; // buffer object
volatile Buffer messageBuffer; // message object

uint8_t msgs_buffer = 0;

uint8_t dataPoint = 0xA; // RXREG

int main() {
    //[1] raw data buffer
    rawBuffer = initBuffer(raw_buffer,sizeof(raw_buffer[0]),sizeof(raw_buffer)/sizeof(raw_buffer[0]));
    //[2] messages buffer
    messageBuffer = initBuffer(msgData,sizeof(msgData[0]),sizeof(msgData)/sizeof(msgData[0]));
    //[3] start message object
    msg M = initMsg(&messageBuffer,&rawBuffer);

    //[4] new data comes to raw buffer
    enq(&dataPoint,&rawBuffer);

    //[5] process the raw buffer and store valid messages in message buffer    
    processMsg(&M);





    printf("Hello, World!");
    return 0;
}