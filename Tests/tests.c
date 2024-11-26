#include <stdio.h>
#include <stdint.h>
#include "msgs.h"
#include "buffers/buffers.h"


// test strings
uint8_t nTests = 2;

const uint8_t test1[6] = {161, 162, 6, 241, 162, 161};

const uint8_t test2[6] = {161, 162, 6, 2, 162, 161};

uint8_t raw_buffer[20];
uint8_t msgData[10];
Msg msg;

uint8_t runTest_1(uint8_t* test, uint8_t sizeTest);
uint8_t runTest_2(uint8_t* test, uint8_t sizeTest);
validOutput myValidation(uint8_t byte ,uint8_t* t1,uint8_t t2, Buffer* buffer);


int main(){
    // runTest_2(test2,sizeof(test2)/sizeof(test2[0]));

    runTest_2(test1, sizeof(test1)/sizeof(test1[0]));
}


uint8_t runTest_1(uint8_t* test, uint8_t sizeTest){
    // loads all the data into buffer then processes it
    uint8_t raw_buffer[20];
    Buffer b_buffer = initBuffer(raw_buffer,20);
    Msg msg = msg; // set a new message
    initMsg(&msg, &b_buffer);

    for (int i = 0; i < sizeTest; i++){
        enq(&test[i],&b_buffer);
    }

    processMsg(&msg);    
    return msg.raw_buffer->msgCount;
}

uint8_t runTest_2(uint8_t* test, uint8_t sizeTest){
    // loads the data one byte at a time and processes it
    uint8_t raw_buffer[20];
    Buffer b_buffer = initBuffer(raw_buffer,20);
    Msg msg = msg; // set a new message
    
    initMsg(&msg, &b_buffer);
    setMsgSize(&msg, 10);
    uint8_t startFlag[2] = {161,162};
    uint8_t endFlag[2] = {162,161};
    
    addValidation(&msg, startFlag, 2);
    addValidation(&msg, endFlag, 2);

    for (int i = 0; i < sizeTest; i++){
        enq(&test[i],&b_buffer);
        processMsg(&msg);   
    }

    return msg.raw_buffer->msgCount;
}

validOutput myValidation(uint8_t byte ,uint8_t* t1,uint8_t t2, Buffer* buffer){
    static uint8_t prevByte = 0;    
    if(prevByte == 162){
        if(byte == 161){
            return OK_move_to_next;
        }
    } 
    prevByte = byte;
    return OK;
}
