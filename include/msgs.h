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
  uint8_t localTemp; // a tempvariable used for each validation function
  uint8_t nStartFlags;
  uint8_t nStartFlagread;
  validOutput (*validationFunction[N_MSG_PARTS])(uint8_t ,const uint8_t*,const uint8_t,Buffer*); // Array of function pointers
  bool isValidationFunction[N_MSG_PARTS];
}Msg;


void initMsg(Msg* msg, Buffer* raw_buffer);
validOutput checkByte(Msg* msg);

bool addValidation(Msg* msg, uint8_t* Flag, uint8_t FlagSize);
bool addValidationFunction(Msg* msg, validOutput (*validationFunction)(uint8_t ,const uint8_t*,const uint8_t,Buffer*));
bool addMsgHolder(Msg* msg, uint8_t maxMsgSize);

void printMsgForm(Msg* msg);

void processMsg(Msg* msg);
bool checkPartN(Msg* msg, uint8_t byte, uint8_t N);

validOutput validateByte(uint8_t byte,const uint8_t* flag, const uint8_t flagSize, Buffer* buffer);

#endif // MSGS_H
/*
const uint8_t startFlag[] = "@WRIST:";
const uint8_t endFlag[] = "*\r";


const uint8_t startFlag_LastIdx = sizeof(startFlag) -2 ;
const uint8_t endFlag_LastIdx = sizeof(endFlag) -2 ;
const uint8_t MAX_MSG_SIZE = 15;

static uint8_t inFlagIndex = 0;
static uint8_t msgBytesLeft = 0;
*/