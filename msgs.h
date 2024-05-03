#ifndef MSGS_H
#define MSGS_H

#include "buffers.h"

typedef struct {
    uint8_t* msgOut;
    uint8_t* msgOutSize;
    uint8_t* msgArray;
    uint8_t* msgSize;
    uint8_t  msgToRead;
    uint8_t  msgToWrite;
    uint8_t  msgsAvailable;
    uint8_t  maxMsgs;
    uint8_t  msgmaxsize;
    bool     isEmpty;
} Msg;

Msg initMsg(uint8_t* msgArray, uint8_t* msgSize, uint8_t maxMsgs, uint8_t msgmaxsize);
void copyRng(uint8_t* dest, uint8_t* src, uint8_t idxStart, uint8_t idxEnd, uint8_t arraySize);
uint8_t getRngSize(uint8_t idxStart, uint8_t idxEnd, uint8_t arraySize);
void storeMsg(Msg* msg, uint8_t* src, uint8_t idxStart, uint8_t idxEnd, uint8_t arraySize);
void getMsg(Msg* msg, uint8_t* dest, uint8_t* sz);
void processMsg(Msg* msg ,volatile Buffer *raw_buffer);
uint8_t calcCS_buffer(volatile Buffer* buffer, uint8_t size);
uint8_t calcCS_array(uint8_t* arr, uint8_t size);
// message structure => [firstStartFlag,secondStartFlag,Size,{DATA[]},firstEndFlag,secondEndFlag]
// void storeMsg(uint8_t* src, size_t sz);
// void getMsg(uint8_t* dest);

#endif // MSGS_H
