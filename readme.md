# msgs

## Overview
The `msgs` package is a communication protocol designed specifically for embedded systems. It provides a lightweight and efficient way to exchange messages between devices.

### default message structure
[`0xA1` `0xA2` `sizeOfPackage` `data` `0xA2` `0xA1`]

This library checks the 0xA1 0xA2 flags, and the correct size of package, if the package is valid the data and its size are moved to an array of choice.
The message is copied to msgOutput and its size is saved in m.msgDataSize.

## dependencies
The package depends on `buffer.h` library [found here](https://github.com/bsyy6/buffers).

## how to use

1) include the following packages in your project.
```c
#include "msgs.h"
#include "buffers.h"
```

2) start three buffered arrays: the first will have raw data, second will have messages and the final will have the messages lengths.

```c
uint8_t raw_buffer[12]; // raw data go here
uint8_t msgData[20];    // extracted msgs go here. (can have more than one message inside)
uint8_t msgs_idxs[3];   // the messages lenghts go here.

volatile Buffer rawBuffer;
volatile Buffer msgDataBuffer;
volatile Buffer msgIdxsBuffer;

// initialize the buffers
//[1] raw data buffer
rawBuffer = initBuffer(raw_buffer,sizeof(raw_buffer[0]),sizeof(raw_buffer)/sizeof(raw_buffer[0]));
//[2] messages buffer
msgDataBuffer = initBuffer(msgData,sizeof(msgData[0]),sizeof(msgData)/sizeof(msgData[0]));
// [3] message index buffer
msgIdxsBuffer = initBuffer(msgs_idxs,sizeof(msgs_idxs[0]),sizeof(msgs_idxs)/sizeof(msgs_idxs[0]));

// make an array where correct messages will be put every time one arrives.
uint_8 msgOutput[10];
```

3) now initialize the message object M
```c
msg M =  initMsg(&msgDataBuffer,&rawBuffer,&msgsIdx,&msgOutput);
```

4) now add data into the rawBuffer as usual.
```c
// this can be in an interrupt
uint_8 dataPoint = 1; // example
enq(&dataPoint,&rawBuffer);
```

5) in your main loop process the buffers looking for messages.
```c
int main(){
    while(1){
    // code goes here


    processMsg(&M); // checks for messages in the raw buffer
    while(m.msgsAvailable){
        getMsg(&M);
        // this will move the correctly recieved message into msgOutput
        // the size of each message is in M->msgDataSize
        yourCustomFunction(msgOutput,m.msgDataSize);
        // every time you call getMsg it updates msgOutput and m.msgDataSize
    }
}

```






