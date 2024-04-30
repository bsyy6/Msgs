#include <stdio.h>
#include "msgs.h"
#include "buffers.h"


uint8_t msgData[20];
uint8_t msgSize = 0;
uint8_t raw_buffer1[12];

uint8_t msgs_idxs[3];
uint8_t msgOutput[10];

volatile Buffer rawBuffer; // buffer object
volatile Buffer messageBuffer; // message object

volatile Buffer msgsIdx; // message index object

uint8_t msgs_buffer = 0;
uint8_t U1RXREG[14] = { 161, 162, 6, 1, 162, 161, 242, 161,  // wrong A2
                        162, 7, 2 ,5, 162, 161}; // wrong A1

//uint8_t U1RXREG[6] = { 161, 162, 6, 241, 162, 161}; // correct

uint8_t dataPoint = 0xA; // RXREG

int main() {
    
    //[1] raw data buffer
    rawBuffer = initBuffer(raw_buffer1,12);

    //[4] new data comes to raw buffer
    int i =0; 
    for (i = 0; i<16;i++)
    {
        if(i%3==0 && i>=3){
            enq(&U1RXREG[i-3],&rawBuffer);
            enq(&U1RXREG[i-2],&rawBuffer);
            enq(&U1RXREG[i-1],&rawBuffer);
            processMsg(&rawBuffer);
        }
    }
    while(rawBuffer.msgCount>0){
        getMsg(&rawBuffer,msgOutput,&msgSize);
    }
    return 0;
}