#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "msgs.h"
#include "buffers/buffers.h"


// test strings
uint8_t nTests = 2;

// simple start flag - end flag no checks in between
const uint8_t test1[6] = {161, 162, 6, 2, 162, 161}; // correct checksum at 5

// simple start flag - end flag  5 is checksum of 161-162-6 
const uint8_t test2[6] = {161, 162, 6,  5, 162, 161};

// start flags - size of Payload {MSB-LSB} - checksum
const uint8_t test3[6] = {161, 162, 0, 1, 6, 4}; // correct checksum at 5

// start flags - size of Payload - checksum - end flags
const uint8_t test4[8] = {161, 162, 0, 1, 6, 4, 162, 161}; // correct checksum at 5

// start flags MIA style 
const uint8_t testMia[9] = {'E','M','G',':', 9, 8, 7, 6, '\n'};

const uint8_t testMia2[9] = {'A','D','C',':', 3, 3, 3, 4, '\n'};

uint8_t raw_buffer[20];
uint8_t msgData[10];
Msg msg;

uint8_t runTest_1(uint8_t* test, uint8_t sizeTest);
uint8_t runTest_2(uint8_t* test, uint8_t sizeTest);
uint8_t runTest_3(uint8_t* test, uint8_t sizeTest);
uint8_t runTest_4_MIA(uint8_t* test, uint8_t sizeTest);

// custom validation functions
ValidationFunction setsSizeOfMsg;
ValidationFunction checksum;




int main(){
    // runTest_3(test3,sizeof(test3)/sizeof(test3[0]));
    runTest_4_MIA(testMia,sizeof(testMia)/sizeof(testMia[0]));
    // runTest_3(test1, sizeof(test1)/sizeof(test1[0]));
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

uint8_t runTest_3(uint8_t* test, uint8_t sizeTest){
    // loads the data one byte at a time and processes it
    // adds some sum function to check it is correct!
    uint8_t raw_buffer[20];
    Buffer b_buffer = initBuffer(raw_buffer,20);
    Msg msg = msg; // set a new message
    
    initMsg(&msg, &b_buffer);
    setMsgSize(&msg, 10);
    uint8_t startFlag[2] = {161,162};
    uint8_t endFlag[2] = {162,161};
    
    addValidation(&msg, startFlag, 2);
    addValidationFunction(&msg, setsSizeOfMsg, true);
    addValidation(&msg, NULL, 0);
    addValidationFunction(&msg, checksum, true);
    // addValidation(&msg, endFlag, 2);

    for (int i = 0; i < sizeTest; i++){
        enq(&test[i],&b_buffer);
        processMsg(&msg);   
    }

    return msg.raw_buffer->msgCount;
}


validOutput checksum(Msg* msg){
    /* for readability */
    uint8_t byte = msg->byte;
    Buffer* buffer = msg->raw_buffer;

    jumpToMsgStart(buffer);
    uint8_t dataInBuffer =howMuchData(buffer); 
    if(dataInBuffer == 0){
        return 0;
    }

    uint8_t cs = 0;
    uint8_t b = 0; // holder for bytes
    for(uint8_t i = 0; i <dataInBuffer-1 ; i++){
        deq(&b, buffer);
        cs^=b;
    }
    deq(&b, buffer);
    
    return (cs == b) ? OK_move_to_next:NOT_OK_GO_TO_ERROR;
}

validOutput setsSizeOfMsg(Msg* msg){
    /* for readability */
    uint8_t byte = msg->byte;

    static uint16_t size = 0;
    static uint8_t idx = 0;
    if(idx == 0){
        size = byte;
        idx++;
        return OK;
    }
    if(idx == 1){
        size = size << 8;
        size |= byte;
        msg->startFlagsSize[2] = (uint8_t)size&0xFF; // 2 is the payload part of message
        return OK_move_to_next;
    }
}


uint8_t runTest_4_MIA(uint8_t* test, uint8_t sizeTest){
    // loads the data one byte at a time and processes it
    // adds some sum function to check it is correct!
    uint8_t raw_buffer[20];
    Buffer b_buffer = initBuffer(raw_buffer,20);
    Msg msg = msg; // set a new message
    
    initMsg(&msg, &b_buffer);
    setMsgSize(&msg, 10);
    uint8_t startFlag = ':';
    uint8_t endFlag = '\n'; // LF
    
    addValidation(&msg, NULL, 4);
    addValidation(&msg, &startFlag, 1);
    addValidation(&msg, &endFlag, 1);

    for (int i = 0; i < sizeTest; i++){
        enq(&test[i],&b_buffer);
        processMsg(&msg);   
    }

    return msg.raw_buffer->msgCount;
}