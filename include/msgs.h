#ifndef MSGS_H
#define MSGS_H

#include "buffers/buffers.h"
#include <stdbool.h>
#include <stdint.h>

#define N_MSG_PARTS 5
typedef enum {
    START1, // waiting for first flag
    CONT,   // continue for the rest of flags
    END1,   // ending flag
    ERROR,  // error
} State;


typedef enum {
    NOT_OK,
    NOT_OK_GO_TO_ERROR,
    OK,
    OK_START_TRACKING,
    OK_move_to_next,
}validOutput;

typedef struct {
  volatile Buffer *raw_buffer;
  State state;
  uint8_t byte;
  uint8_t* startFlags[N_MSG_PARTS];
  uint8_t startFlagsSize[N_MSG_PARTS];
  bool isCustomValidation[N_MSG_PARTS];
  uint8_t nBytesInCurrentMsg; // the number of bytes in the currently underporcess message.
  uint8_t nBytesInCurrentMsg_MAX;
  uint8_t nStartFlags;
  uint8_t nStartFlagread;
  validOutput (*validationFunction[N_MSG_PARTS])(uint8_t , uint8_t*, uint8_t, volatile Buffer*); // Array of function pointers
  bool isValidationFunction[N_MSG_PARTS];
}Msg;


void initMsg(Msg* msg, Buffer* raw_buffer);
validOutput checkByte(Msg* msg);

bool addValidation(Msg* msg, uint8_t* Flag, uint8_t FlagSize);
bool addValidationFunction(Msg* msg, validOutput (*validationFunction)(uint8_t , uint8_t*, uint8_t,volatile Buffer*));
State handleStateTransiiton(Msg* msg, validOutput output);
void printMsgForm(Msg* msg);

void processMsg(Msg* msg);
bool checkPartN(Msg* msg, uint8_t byte, uint8_t N);
bool setMsgSize(Msg* msg, uint8_t size);


validOutput validateByte(uint8_t byte, uint8_t* flag,  uint8_t flagSize,volatile Buffer* buffer);

#endif // MSGS_H
