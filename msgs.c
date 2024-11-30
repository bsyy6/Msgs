/* created by Waleed - 2024 
    A simple message library that takes a buffer (check buffers.h) and processes the messages in it.
    it discards any bytes that don't belong to a message.
    keeps messages on the buffer and marks the start and end of each message.
*/

#include "msgs.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


void processMsg(Msg* msg){
    
    // Reads through the buffer: if a valid message is found it:
    // [1] freezes its locaiton in the buffer so it is not overwritten.
    // [2] stores the location of the message in the buffer->msgRanges
    
    // to retrieve the message, use the 
    // void getMsg(volatile Buffer *buffer, uint8_t* msgOut); 
    // it will copy the message to msgOut array and unfreeze the buffer to work as usual.
    // you can check if messages are available by checking buffer->msgCount.

    static uint8_t counterMsg = 0;
    if(msg->raw_buffer->isEmpty){
        return;
    }
    while(!msg->raw_buffer->isEmpty){
        deq(&msg->byte,msg->raw_buffer);
        validOutput output = checkByte(msg);
        msg->state = handleStateTransiiton(msg,output);

        switch (msg->state){
            case START1:
                if(output == OK_START_TRACKING){
                    if(!msg->raw_buffer->Blocked){
                        setMsgStart(msg->raw_buffer);
                        msg->nBytesInCurrentMsg = 1;
                    }             
                }
                break;

            case CONT:
                msg->nBytesInCurrentMsg++;
                break;       

            case END1:
                enqMsg(msg->raw_buffer);
                msg->state = START1;
                
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

void initMsg(Msg* msg, Buffer* raw_buffer){
    msg->raw_buffer = raw_buffer;
    msg->state = START1;
    msg->nStartFlags = 0;
    msg->nStartFlagread = 0;
    msg->nBytesInCurrentMsg = 0;
    msg->nBytesInCurrentMsg_MAX = raw_buffer->arraySize/2;
}

bool addValidation(Msg* msg, uint8_t* startFlag, uint8_t startFlagSize){
    if(msg->nStartFlags < N_MSG_PARTS){
        
        if( addValidationFunction(msg, validateByte, false)){
            msg->startFlags[msg->nStartFlags-1] = startFlag;
            msg->startFlagsSize[msg->nStartFlags-1] = startFlagSize;
            return true;
        }
    }
    return false;
}

validOutput checkByte(Msg* msg){
    validOutput output;

    // run the validation funciton
    if(msg->isCustomValidation[msg->nStartFlagread]){
       output = msg->validationFunctions[msg->nStartFlagread](msg);
    }else{
        output = validateByte(msg);
    }

    if(output ==  OK_move_to_next){
        msg->nStartFlagread++;
    }

    return output;

}

/** @brief default validation function, goes through flag array and returns ok_move_to_next validation if 
* if the flag was found in the byte sequences.
 */
validOutput validateByte(struct Msg* msg){
    static uint8_t idx = 0;
    validOutput output = OK;
    /* for easier reading */
    uint8_t byte = msg->byte;
    uint8_t* flag = msg->startFlags[msg->nStartFlagread];
    uint8_t flagSize = msg->startFlagsSize[msg->nStartFlagread];

    // not poiting to any flag
    if(flag == NULL){
        idx++;
        output = OK;
    }else{
        // pointing to a flag
        if(byte == flag[idx] ){
            output = (idx == 0) ? OK_START_TRACKING:OK;
            idx++;
        }else if (idx > 0){
            idx = 0;
            output = NOT_OK_GO_TO_ERROR;
        }
    }
    // all flag is ok
    if(idx == flagSize){
        idx = 0;
        output = OK_move_to_next;
    }

    return output;
}


// TODO
bool addValidationFunction(Msg* msg, validOutput (*validationFunction)(Msg* msg), bool isCustom){
    if(msg->nStartFlags < N_MSG_PARTS){
        msg->validationFunctions[msg->nStartFlags] = validationFunction;
        msg->isCustomValidation[msg->nStartFlags] = isCustom;
        msg->startFlags[msg->nStartFlags] = NULL;
        msg->startFlagsSize[msg->nStartFlags] = 0;
        msg->nStartFlags++;
        return true;
    }
    return false;
}

State handleStateTransiiton(Msg* msg, validOutput output){
    State state = msg->state;
    if(output == OK_START_TRACKING )
        state = state;

    if(output == OK_move_to_next)
        state = CONT;
    
    if( output == NOT_OK_GO_TO_ERROR ||
        msg->nBytesInCurrentMsg > msg->nBytesInCurrentMsg_MAX)
        state = ERROR;
    
    if(msg->nStartFlagread == msg->nStartFlags)
        state = END1; // passed all validation functions!
    
    return state;
}


bool setMsgSize(Msg* msg, uint8_t size){
    if(size < msg->raw_buffer->arraySize){
        msg->nBytesInCurrentMsg_MAX = size;
        return true;
    }else{
        return false;
    }
}

