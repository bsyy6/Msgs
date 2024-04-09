#include "msgs.h"
#include "buffers.h"

msg initMsg( volatile Buffer *msgs_buffer, volatile Buffer *raw_buffer){
    msg M;
    M.rawBuffer = raw_buffer;
    M.msgQueue = msgs_buffer;
    M.state = START1;
    M.msgSize = 0;
    M.msgArrived = false;
    M.byte = 0;
    M.prevByte = 0;
    M.state = START1;
    M.prevState = START1;
    M.size = 0;
    M.msgCount = 0;
    M.startSearch = 0;
    return M;
}


void processMsg(msg *m){
    if(m->rawBuffer->isEmpty){
        return;
    }
    // update history
    m->prevByte  = m->byte;
    m->prevState = m->state;
    if(!m->rawBuffer->isEmpty){
        deq(&m->byte,m->rawBuffer);   
    }
}