/* created by Waleed - 2024 
    A simple message library that takes a buffer (check buffers.h) and processes the messages in it.
    it discards any bytes that don't belong to a message.
    keeps messages on the buffer and marks the start and end of each message.
*/

#include "msgs.h"


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
            if(findNextMsgStart(msg->raw_buffer)){
                jumpToMsgStart(msg->raw_buffer);
            }else{
                removeMsgStart(msg->raw_buffer); // free up the buffer for overwrites.
            }
            msg->state = START1;
        }   
    }
}


bool addValidation(Msg* msg, uint8_t* startFlag, uint8_t startFlagSize){
    if(msg->nStartFlags < N_MSG_PARTS){
        addValidationFunction(msg, validateByte);
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
       output = msg->validationFunction[msg->nStartFlagread](msg->byte,msg->startFlags[msg->nStartFlagread],msg->startFlagsSize[msg->nStartFlagread], msg->raw_buffer);
    }else{
        output = validateByte(msg->byte, msg->startFlags[msg->nStartFlagread],msg->startFlagsSize[msg->nStartFlagread],msg->raw_buffer);
    }

    if(output ==  OK_move_to_next){
        msg->nStartFlagread++;
        if(msg->nStartFlagread == msg->nStartFlags){
            msg->state = END1; // passed all validation functions!
        }
    }

    if(output == NOT_OK){
        msg->state = ERROR;
    }
    return output;

}

/** @brief default validation function, goes through flag array and returns ok_move_to_next validation if 
* if the flag was found in the byte sequences.
 */
validOutput validateByte(uint8_t byte,uint8_t* flag, uint8_t flagSize, Buffer* buffer){
    static uint8_t idx = 0;
    if(byte == flag[idx]){
        if(idx == flagSize-1){
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
bool addValidationFunction(Msg* msg, validOutput (*validationFunction)(uint8_t byte, const uint8_t* flag,const uint8_t flagSize,Buffer* buffer)){
    if(msg->nStartFlags < N_MSG_PARTS){
        msg->validationFunction[msg->nStartFlags] = validationFunction;
        msg->nStartFlags++;
        return true;
    }
    return false;
}

