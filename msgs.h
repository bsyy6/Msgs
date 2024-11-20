#ifndef MSGS_H
#define MSGS_H

#include "buffers.h"
#include <stdbool.h>
#include <stdint.h>
void processMsg(volatile Buffer *raw_buffer);

#endif // MSGS_H
