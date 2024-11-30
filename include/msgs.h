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

struct Msg;

// define validatain function signature
typedef validOutput ValidationFunction(struct Msg* msg);
typedef struct Msg {
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
  ValidationFunction* validationFunctions[N_MSG_PARTS]; // Array of function pointers
}Msg;

void initMsg(Msg* msg, Buffer* raw_buffer);


bool addValidation(Msg* msg, uint8_t* Flag, uint8_t FlagSize);
bool addValidationFunction(Msg* msg, ValidationFunction, bool isCustomValidation);
State handleStateTransiiton(Msg* msg, validOutput output);
void printMsgForm(Msg* msg);

void processMsg(Msg* msg);
bool setMsgSize(Msg* msg, uint8_t size);

ValidationFunction checkByte;
ValidationFunction validateByte;

#endif // MSGS_H
