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
                if(output == OK_START_TRACKING){
                    if(!msg->raw_buffer->Blocked){
                        setMsgStart(msg->raw_buffer);
                    }             
                }else if(output == OK_move_to_next){ 
                    msg->state = CONT;
                }else if(output == NOT_OK_GO_TO_ERROR){
                    msg->state = ERROR;
                }
                
                break; 
            case CONT:
                if (output == OK_move_to_next){
                    msg->state = END1;
                }else if (output == NOT_OK_GO_TO_ERROR){
                    msg->state = ERROR;
                }
                break;            
            case END1:
                if(output == OK_move_to_next){
                    enqMsg(msg->raw_buffer);
                    //reset msg->state to get next message
                    msg->state = START1;
                }else if(output == NOT_OK_GO_TO_ERROR){
                    msg->state = ERROR;
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

void initMsg(Msg* msg, Buffer* raw_buffer){
    msg->raw_buffer = raw_buffer;
    msg->state = START1;
    msg->nStartFlags = 0;
    msg->nStartFlagread = 0;
}

bool addValidation(Msg* msg, uint8_t* startFlag, uint8_t startFlagSize){
    if(msg->nStartFlags < N_MSG_PARTS){
        if( addValidationFunction(msg, validateByte)){;
            msg->isCustomValidation[msg->nStartFlags] = false;
            msg->startFlags[msg->nStartFlags] = startFlag;
            msg->startFlagsSize[msg->nStartFlags] = startFlagSize;
            msg->nStartFlags++;
            return true;
        }
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
validOutput validateByte(uint8_t byte,const uint8_t* flag, const uint8_t flagSize, Buffer* buffer){
    static uint8_t idx = 0;
    validOutput output = OK;

    // correct flag
    if(byte == flag[idx] ){
        output = (idx == 0) ? OK_START_TRACKING:OK;
        idx++;
    }else if (idx > 0){
        idx = 0;
        output = NOT_OK_GO_TO_ERROR;
    }
    
    // all flag is ok
    if(idx == flagSize){
        idx = 0;
        output = OK_move_to_next;
    }

    return output;
}

// TODO
bool addValidationFunction(Msg* msg, validOutput (*validationFunction)(uint8_t byte, const uint8_t* flag,const uint8_t flagSize,Buffer* buffer)){
    if(msg->nStartFlags < N_MSG_PARTS){
        msg->validationFunction[msg->nStartFlags] = validationFunction;
        return true;
    }
    return false;
}

validOutput addLastValidation(uint8_t byte,const uint8_t* flag, const uint8_t flagSize, Buffer* buffer){
    validOutput output = OK;
    static uint8_t idx = 0;

    // correct flag
    if(byte == flag[idx]){
        output = (idx == 0) ? OK_START_TRACKING:OK;
        idx++;
    }else{
        idx = 0;
    }
    
    // all flag is ok
    if(idx == flagSize){
        idx = 0;
        output = OK_move_to_next;
    }

    return output;
}
