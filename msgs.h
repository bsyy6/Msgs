#ifndef MSGS_H
#define MSGS_H

#include "buffers.h"


// message structure => [firstStartFlag,secondStartFlag,Size,{DATA[]},firstEndFlag,secondEndFlag]
void processMsg(volatile Buffer *raw_buffer);


// message structure => [firstStartFlag,secondStartFlag,Size[2 bytes],{DATA[]},checksum]
void processMsgBluetooth(volatile Buffer *raw_buffer, uint8_t secondStartFlag);
uint8_t calcCS_buffer(Buffer* buffer, uint8_t size);

#endif // MSGS_H
