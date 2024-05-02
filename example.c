#include <stdio.h>
#include "msgs.h"
#include "buffers.h"


uint8_t msgData[20];
uint8_t msgSize = 0;
uint8_t raw_buffer1[21];

uint8_t msgs_idxs[3];
uint8_t msgOutput[12];

volatile Buffer rawBuffer; // buffer object
volatile Buffer messageBuffer; // message object

volatile Buffer msgsIdx; // message index object

uint8_t msgs_buffer = 0;
uint8_t U1RXREG[6] = {161, 162, 6, 2, 162, 161}; // wrong A1
//uint8_t U1RXREG[30] = {0x01, 0x02, 0x50, 0x07, 0x00, 0x00, 0x55, 0x00, 0x00, 0xDA, 0x18, 0x00, 0xB2,0x02, 0x50, 0x07, 0x00, 0x00, 0x55, 0x00, 0x00, 0xDA, 0x18, 0x00, 0xC2} ; 

//uint8_t U1RXREG[6] = { 161, 162, 6, 241, 162, 161}; // correct

uint8_t dataPoint = 0xA; // RXREG

int main() { 
    //[1] raw data buffer
    rawBuffer = initBuffer(raw_buffer1,21);

    // [4] new data comes to raw buffer
    int i =0; 
    int j = 0;
    for (j=0;j<3;j++){
        for (i = 0; i<14;i++)
        {
            if(i%3==0 && i>=3){
                enq(&U1RXREG[i-3],&rawBuffer);
                enq(&U1RXREG[i-2],&rawBuffer);
                enq(&U1RXREG[i-1],&rawBuffer);
                processMsg(&rawBuffer);
            }
        }
    }
    if(rawBuffer.msgCount>0){
        getMsg(&rawBuffer,msgOutput,&msgSize);
    }
    
   return 0;
}