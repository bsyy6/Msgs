/* created by Waleed - 2024 
    A simple message library that takes a buffer (check buffers.h) and processes the messages in it.
    it discards any bytes that don't belong to a message.
    keeps messages on the buffer and marks the start and end of each message.
*/

#include "msgs.h"
#include <stdbool.h>
#include <stdint.h>

void processMsg(Msg* msg){
    
    // Reads through the buffer: if a valid message is found it:
    // [1] freezes its locaiton in the buffer so it is not overwritten.
    // [2] stores the location of the message in the buffer->msgRanges
    
    // to retrieve the message, use the 
    // void getMsg(volatile Buffer *buffer, uint8_t* msgOut); 
    // it will copy the message to msgOut array and unfreeze the buffer to work as usual.
    // you can check if messages are available by checking buffer->msgCount.

    
    if(msg->raw_buffer->isEmpty){
        return;
    }
    while(!msg->raw_buffer->isEmpty){
        deq(&msg->byte,msg->raw_buffer);
        validOutput output = checkByte(msg);

        switch (msg->state){
            case START1:
                if(output == OK_move_to_next){
                    setMsgStart(msg->raw_buffer);
                    msg->state = CONT;
                }
                break; 
            case CONT:
                // do nothing
                break;            
            case END1:
                if(output == OK_move_to_next){
                    enqMsg(msg->raw_buffer);
                    //reset msg->state to get next message
                    msg->state = START1;
                }
                break;
            case ERROR:
                // do nothing
                break;
        }
        
        // directly enforce error msg->state handling
        if(msg->state == ERROR){
            if(findNextMsgStart(raw_buffer)){
                jumpToMsgStart(raw_buffer);
            }else{
                removeMsgStart(raw_buffer); // free up the buffer for overwrites.
            }
            msg->state = START1;
        }   
    }
}


bool addValidation(Msg* msg, uint8_t* startFlag, uint8_t startFlagSize){
    if(msg->nStartFlags < N_MSG_PARTS){
        addValidationFunction(msg, validateBytes);
        msg->isCustomValidation[msg->nStartFlags] = false;
        msg->startFlags[msg->nStartFlags] = startFlag;
        msg->startFlagsSize[msg->nStartFlags] = startFlagSize;
        msg->nStartFlags++;
        return true;
    }
    return false;
}

validOutput checkByte(Msg* msg){
    validOutput output;

    // run the validation funciton
    if(msg->isCustomValidation[msg->nStartFlagread]){
       output = msg->validationFunction[msg->nStartFlagread](msg->byte,msg->startFlags[msg->nStartFlagread],msg->startFlagsSize[msg->nStartFlagread]);
    }else{
        output = validateByte(msg->byte, msg->startFlags[msg->nStartFlagread],msg->startFlagsSize[msg->nStartFlagread]);
    }

    if(output ==  OK_move_to_next)
        msg->nStartFlagread++;
        if(msg->nStartFlagread == msg->nStartFlags){
            msg->state = END1;
        }
    if(output == NOT_OK){
        msg->state = ERROR;
    }
    return output;

}

validOutput validateByte(uint8_t byte,uint8_t* startFlag, uint8_t startFlagsSize){
    static uint8_t idx = 0;
    if(byte == startFlag[idx]){
        if(idx == startFlagsSize-1){
            idx = 0;
            return OK_move_to_next;
        }
        idx++;
        return OK;
    }
    idx= 0;
    return NOT_OK;
}

// TODO
bool addValidationFunction(Msg* msg, validOutput (*validationFunction)(uint8_t,uint8_t*,uint8_t)){
    if(msg->nStartFlags < N_MSG_PARTS){
        msg->validationFunction[msg->nStartFlags] = validationFunction;
        msg->nStartFlags++;
        return true;
    }
    return false;
}

