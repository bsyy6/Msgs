/* created by Waleed - 2024 
    A simple message library that takes a buffer (check buffers.h) and processes the messages in it.
    it discards any bytes that don't belong to a message.
    keeps messages on the buffer and marks the start and end of each message.
*/

#include "msgs.h"

void processMsg(volatile Buffer *raw_buffer){
    
    // Reads through the buffer: if a valid message is found it:
    // [1] freezes its locaiton in the buffer so it is not overwritten.
    // [2] stores the location of the message in the buffer->msgRanges
    
    // to retrieve the message, use the 
    // void getMsg(volatile Buffer *buffer, uint8_t* msgOut); 
    // it will copy the message to msgOut array and unfreeze the buffer to work as usual.
    // you can check if messages are available by checking buffer->msgCount.

    typedef enum {
        START1, // waiting for first flag
        START2,
        COMMAND, // getting data
        END1,
        ERROR, // error
    } State;
    
    
    // message structure => [@WRIST:,{DATA[]},"*\r"]
    /*
    *  @WRIST:STOP*[invio] : ferma il motore
    *  @WRIST:Fxxx*[invio] : gira in un verso con velocita' indicata (al posto di xxx)
    *  @WRIST:Rxxx*[invio] : gira nel verso opposto
    * Invia ASCII da Real Term, al posto di [invio] metti \r (=0x0d)
    */
    
    const uint8_t startFlag[] = "@WRIST:";
    const uint8_t endFlag[] = "*\r";

    
    const uint8_t startFlag_LastIdx = sizeof(startFlag) -2 ;
    const uint8_t endFlag_LastIdx = sizeof(endFlag) -2 ;
    const uint8_t MAX_MSG_SIZE = 15;

    static uint8_t byte;
    static uint8_t inFlagIndex = 0;
    static State state = START1;
    static uint8_t msgBytesLeft = 0;

    if(raw_buffer->isEmpty){
        return;
    }
    while(!raw_buffer->isEmpty){
        deq(&byte,raw_buffer);
        switch (state){
            case START1:
                if (byte == startFlag[0]) {
                    setMsgStart(raw_buffer);
                    inFlagIndex=1;
                    state = START2;
                }else{
                    state = ERROR;
                }
                break;
            case START2:
                if (byte == startFlag[inFlagIndex]) {
                    inFlagIndex++;
                    state = (inFlagIndex == startFlag_LastIdx+1)? COMMAND:state;
                }else{
                    state = ERROR;
                }
                break;
            case COMMAND:
                if(byte == endFlag[0]){
                    state = END1;
                    inFlagIndex = 1;
                }
                break;
            case END1:
                if(byte == endFlag[inFlagIndex]){
                    inFlagIndex++;
                    if(inFlagIndex == endFlag_LastIdx+1){
                        enqMsg(raw_buffer);
                        //reset state to get next message
                        inFlagIndex = 0;
                        state = START1;
                    }
                }else{
                    state = ERROR;
                }
                break;
            case ERROR:
                // do nothing
                break;
        }
        
        // directly enforce error state handling
        if(state == ERROR){
            inFlagIndex = 0;
            if(findNextMsgStart(raw_buffer)){
                jumpToMsgStart(raw_buffer);
            }else{
                removeMsgStart(raw_buffer); // free up the buffer for overwrites.
            }
            state = START1;
        }   
    }
}
