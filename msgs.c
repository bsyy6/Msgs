

// msg initMsg(uint8_t *data, uint8_t size){
//     msg M;
//     M.msgQueue = initBuffer(data,size);
//     M.state = START1;
//     M.msgSize = 0;
//     M.msgArrived = false;
//     M.byte = 0;
//     M.prevByte = 0;
//     M.state = START1;
//     M.prevState = START1;
//     M.size = 0;
//     M.msgCount = 0;
//     M.startSearch = 0;
//     return M;
// }


// void processMsg(msg *m, volatile Buffer *buffer){
//     if(buffer->isEmpty){
//         return;
//     }
//     // update history
//     m->prevByte  = m->byte;
//     m->prevState = m->state;
//     while(!buffer->isEmpty){
//         m->byte = buffer->data[buffer->tail];
//         buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
//         if(1==1){
//         writeBuffer(m->byte,m->msgQueue);
//         }
//     }
// }