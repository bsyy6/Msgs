/* created by Waleed - 2024 
    A simple message library that takes a buffer (check buffers.h) and processes the messages in it.
    it discards any bytes that don't belong to a message.
    keeps messages on the buffer and marks the start and end of each message.
*/

#include "msgs.h"
#include "buffers.h"

void processMsg(volatile Buffer *raw_buffer){
    
    // Reads through the buffer: if a valid message is found it:
    // [1] freezes its locaiton in the buffer so it is not overwritten.
    // [2] stores the location of the message in the buffer->msgRanges
    
    // to retrieve the message, use the 
    // void getMsg(volatile Buffer *buffer, uint8_t* msgOut); 
    // it will copy the message to msgOut array and unfreeze the buffer to work as usual.
    // you can check if messages are available by checking buffer->msgCount.

    
    // message structure => [firstStartFlag,seocondStartFlag,Size,{DATA[]},firstEndFlag,firstStartFlag]
    const uint8_t firstStartFlag = 0xA1;
    const uint8_t secondStartFlag = 0xA2;
    const uint8_t firstEndFlag = 0xA2;
    const uint8_t secondEndFlag = 0xA1;
    const uint8_t MAX_MSG_SIZE = 15;
    const uint8_t MIN_MSG_SIZE = 7;
    const uint8_t NUM_OF_FLAGS = 4;

    static uint8_t byte;
    static State state = START1;
    static uint8_t msgBytesLeft = 0;

    if(raw_buffer->isEmpty){
        return;
    }
    while(!raw_buffer->isEmpty){
        deq(&byte,raw_buffer);
        switch (state){
            case START1:
                if(byte == firstStartFlag){
                    state = START2;
                    msgBytesLeft = 0;
                    setMsgStart(raw_buffer);
                }
                break;
            case START2:
                if(byte == secondStartFlag){
                    state = SIZE;
                }else{
                    state = ERROR;
                }
                break;
            case SIZE:
                if(byte > MAX_MSG_SIZE || byte < MIN_MSG_SIZE){
                    state = ERROR;
                }else{
                    msgBytesLeft = byte - (NUM_OF_FLAGS+1);// +1 for size byte
                    state = DATA;
                }
                break;
            case DATA:
                msgBytesLeft--; 
                if (msgBytesLeft == 0){
                    state = END1;
                }
                break;
            case END1:
                if(byte == firstEndFlag){
                    state = END2;
                }else{
                    state = ERROR;
                }
                break;
            case END2:
                if(byte == secondEndFlag){
                    markMsg(raw_buffer);
                    state = START1;
                }else{
                    state = ERROR;
                }
                break;
                
            case ERROR:
                if(findNextMsgStart(raw_buffer)){
                    state = START2;
                    byte = firstStartFlag;
                    jumpToMsgStart(raw_buffer);
                }else{
                    removeMsgStart(raw_buffer); // free up the buffer for overwrites.
                    state = START1;
                }
                break;
        }   
    }
}

void processMsgBLuetooth(volatile Buffer *raw_buffer){
    
    // Reads through the buffer: if a valid message is found it:
    // [1] freezes its locaiton in the buffer so it is not overwritten.
    // [2] stores the location of the message in the buffer->msgRanges
    
    // to retrieve the message, use the 
    // void getMsg(volatile Buffer *buffer, uint8_t* msgOut); 
    // it will copy the message to msgOut array and unfreeze the buffer to work as usual.
    // you can check if messages are available by checking buffer->msgCount.
    
    
    // message structure => [firstStartFlag,seocondStartFlag,Size,{DATA[]},firstEndFlag,firstStartFlag]
    const uint8_t firstStartFlag = 0xA1;
    const uint8_t secondStartFlag = 0xA2;
    const uint8_t firstEndFlag = 0xA2;
    const uint8_t secondEndFlag = 0xA1;
    const uint8_t MAX_MSG_SIZE = 15;
    const uint8_t MIN_MSG_SIZE = 7;
    const uint8_t NUM_OF_FLAGS = 4;

    static uint8_t byte;
    static State state = START1;
    static uint8_t msgBytesLeft = 0;

    if(raw_buffer->isEmpty){
        return;
    }
    while(!raw_buffer->isEmpty){
        deq(&byte,raw_buffer);
        switch (state){
            case START1:
                if(byte == firstStartFlag){
                    state = START2;
                    msgBytesLeft = 0;
                    setMsgStart(raw_buffer);
                }
                break;
            case START2:
                if(byte == secondStartFlag){
                    state = SIZE;
                }else{
                    state = ERROR;
                }
                break;
            case SIZE:
                if(byte > MAX_MSG_SIZE || byte < MIN_MSG_SIZE){
                    state = ERROR;
                }else{
                    msgBytesLeft = byte - (NUM_OF_FLAGS+1);// +1 for size byte
                    state = DATA;
                }
                break;
            case DATA:
                msgBytesLeft--; 
                if (msgBytesLeft == 0){
                    state = END1;
                }
                break;
            case END1:
                if(byte == firstEndFlag){
                    state = END2;
                }else{
                    state = ERROR;
                }
                break;
            case END2:
                if(byte == secondEndFlag){
                    markMsg(raw_buffer);
                    state = START1;
                }else{
                    state = ERROR;
                }
                break;
                
            case ERROR:
                if(findNextMsgStart(raw_buffer)){
                    state = START2;
                    byte = firstStartFlag;
                    jumpToMsgStart(raw_buffer);
                }else{
                    removeMsgStart(raw_buffer); // free up the buffer for overwrites.
                    state = START1;
                }
                break;
        }   
    }
}