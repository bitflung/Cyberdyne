#include "c_adup_protocol.h"
//#include "uart_inst.h"
//#include <common/adi_timestamp.h>

C_PROTOCOL _prot;
msg_t _ackMsg;
msg_t _txMsg;

C_PROTOCOL* C_PROTOCOL_init(int baud){
  // functions
  _prot.timeoutTest = &c_protocol_timeoutTest;
  _prot.timeoutHeaderTest = &c_protocol_timeoutHeaderTest;
  // _prot.checkFifo = ;
  _prot.checkFifo = &c_protocol_checkFifo;
  
  _prot.defaultInit = &c_protocol_defaultInit;
//
//  _prot.autobaudStart = &c_protocol_autobaudStart;
//  _prot.autobaudTest = &c_protocol_autobaudTest;
//  _prot.autobaudEnd = &c_protocol_autobaudEnd;
//
//  _prot.setBaud = &c_protocol_setBaud;
  _prot.protocolInit = &c_protocol_protocolInit;
  
  _prot.getListenPend = &c_protocol_getListenPend;
  _prot.clearListenPend = &c_protocol_clearListenPend;
  _prot.getDeadComms = &c_protocol_getDeadComms;
  _prot.clearDeadComms = &c_protocol_clearDeadComms;
  
  _prot.RxProtHandler = &c_protocol_RxProtHandler;
  // _prot.TxProtHandler = TxProtHandler;
  _prot.TxProtHandler = &c_protocol_TxProtHandler;
  _prot.error = &c_protocol_error;
  
  _prot.getHeader = &c_protocol_getHeader;
  _prot.getContent = &c_protocol_getContent;
  
  _prot.putHeader = &c_protocol_putHeader;
  _prot.putContent = &c_protocol_putContent;
  
  // fields
  _prot._comms = C_COMMS_init(baud);
  
  // call the default init function to setup the default msg values
  _prot.defaultInit();
  
  // _prot.listenPending = false;
  // _prot.deadComms = false;
  
  // _prot.PROT_MAX_PAYLOAD = DEFAULT_PROT_MAX_PAYLOAD;
  // _prot._ackMsg.bsize = 0;
  // _prot._txMsg.bsize = 0;
  
  return &_prot;
}


// c_protocol_ADUP_PROTOCOL() : _comms(false)
// {
// defaultInit();
// }

// c_protocol_ADUP_PROTOCOL(int baud) : _comms(false)
// {
// defaultInit();
// if(baud >= 0){
// setBaud(baud);
// }
// }

//bool c_protocol_checkFifo(void){
//  return _prot._comms->checkFifo();
//}

bool c_protocol_checkFifo(uint32_t expected){
  return _prot._comms->checkFifo(expected);
}
//
//void c_protocol_autobaudStart(void){
//  _prot._comms->autobaudStart();
//}
//
//uint32_t c_protocol_autobaudTest(void){
//  return _prot._comms->autobaudTest();
//}
//
//void c_protocol_autobaudEnd(void){
//  _prot._comms->autobaudEnd();
//}
//
//void c_protocol_setBaud(int baud){
//  _prot._comms->setBaud(baud);
//}

void c_protocol_defaultInit(){
  _prot.PROT_MAX_PAYLOAD=DEFAULT_PROT_MAX_PAYLOAD;
  _prot._ackMsg.bsize=0;
  _prot._txMsg.bsize=0;
  _prot.deadComms = false;
  _prot.listenPending=false;
}

void c_protocol_protocolInit(int newsize){
  _prot.PROT_MAX_PAYLOAD=newsize;
}

void c_protocol_getHeader(msg_t *msg){  // get 3 chars [cmd, len]
  char tmp[4]={'\r', '\r', '\r', '\0'};
  char resp[3]="OK";
  // read in a loop just in case the PC side sends a new autobaud character
  // PC will send "\r\r\r" when in autobaud function, so either 1x is used to autobaud then 2x are drained as junk, or all 3 are drained as junk here
  // autobaud response is "OK", sent by autobaud function. here we send "OK" to indicate current baud rate is working properly
  do {
    _prot._comms->getString(tmp, 3);
    
    if(tmp[0]=='\r'){
      _prot._comms->putString(resp, 2); // TODO: my god, did i really make ANY header that starts with \r become an autobaud?!
    }
    
  } while(tmp[0]=='\r');
  
  msg->cmd=tmp[0];
  msg->len=strtol(tmp+1, NULL, 16);
}

void c_protocol_getContent(msg_t *msg){  // get [len] characters content
  if(msg->len > 0) 
    _prot._comms->getString(msg->buf, msg->len); // fill the msg with some payload
}

void c_protocol_putHeader(msg_t *msg) { // put [ack, len] chars
  char tmp[4];  // [0]:cmd [1:2]len, [3]:null-term
  sprintf(tmp, "%c%02x", msg->cmd, msg->len);
  _prot._comms->putString(tmp, 3);
}

void c_protocol_putContent(msg_t *msg) { // put [len] characters content
  if(msg->len > 0)
    _prot._comms->putString(msg->buf, msg->len);
}


bool c_protocol_timeoutHeaderTest(){
  char jnk[3];
  
  do{
    if(_prot.timeoutTest(3)){
      return true; // time out if we don't see data in time
    }
    
    // if we DO see data in time, check whether it is just emulated autobaud
    if(_prot._comms->headerPeek()){
      _prot._comms->getString(jnk, 3);
      _prot._comms->putString("OK", 2);
    }
    else{
      // if we got here then we have a header and didn't timeout
      return false;
    }
    
  } while(true);
}

/**
* returns true if the uart read times out
*/
bool c_protocol_timeoutTest(uint32_t expectedSize){
#ifdef USE_COMMS_TIMEOUTS
  double start;
  double current;
  double diff;
  
  if(expectedSize==0)
    return false;
  
  start = (double)GET_TIME();
  while(! _prot.checkFifo(expectedSize)){
    // each iteration we check if the ACK has arrived - if it has we jump below and handle it properly
    current = (double)GET_TIME();
    diff = current - start;
    if(diff > ACK_TIMEOUT_MS){
      // defined as something like 100ms
      // if we don't get an ack within this timeframe, set a field to indicate DEAD COMMS and get out of here
      _prot.deadComms = true;
      return true;
    }
  }
  
  return false;
#else
  return false; // always return false if we aren't using timeouts
#endif
}

void c_protocol_RxProtHandler(msg_t *msg) {
//  double timeStamp;
  // purge the existing msg
  msg->buf[0]='\0';
  msg->len=0;
  msg->cmd='\0';
  
  _prot._ackMsg.buf = msg->buf;
  _prot._ackMsg.len=0;
  _prot._ackMsg.cmd='\0';
  
  int pending=0;
  
  do {
    // JTE: implementing a timeout feature here so if the other side stops responding, we stop waiting for ACKs
    
    if(_prot.timeoutHeaderTest()){
      return;
    }
    _prot.getHeader(& _prot._ackMsg);
    
    if(_prot._ackMsg.len > _prot.PROT_MAX_PAYLOAD) {
      _prot._ackMsg.len=_prot.PROT_MAX_PAYLOAD;
      pending=1;
    } else {
      pending=0;
    }
    
    // if this payload would cause us to overflow the msg buffer, return an error header and exit the Rx loop
    if((_prot._ackMsg.len + msg->len) > msg->bsize){
      _prot.error("halted on pending buffer overflow in RxProtHandler");
      while(1); // die here for ease of debug
      return;
    }
    
    if(_prot.timeoutTest(_prot._ackMsg.len)){
      return;
    }
    _prot.getContent(&_prot._ackMsg); // only operates if _ackMsg.len > 0
    msg->len+=_prot._ackMsg.len;
    
    _prot._ackMsg.buf=&(_prot._ackMsg.buf[_prot._ackMsg.len]); // move the buf pointer
    _prot._ackMsg.len=0; // prep for ACK header
    
    _prot.putHeader(&_prot._ackMsg); // always return 0 length payload during RX
    
  } while (pending>0); // final loop has tmpLen <= 8 and completes the RX process
  
  msg->buf[msg->len]='\0';
  msg->cmd=_prot._ackMsg.cmd;
}

bool c_protocol_getListenPend(){
  return _prot.listenPending;
}

void c_protocol_clearListenPend(){
  _prot.listenPending=false;
}

bool c_protocol_getDeadComms(){
  return _prot.deadComms;
}

void c_protocol_clearDeadComms(){
  _prot._comms->flush();
  _prot.deadComms=false;
  _prot.clearListenPend();
}

void c_protocol_TxProtHandler(msg_t *msg, bool useAcks){
  // each iteration:
  // MCU: putHeader -> putContent -> getHeader(ack,0)
  // PC:  getHeader -> getContent -> putHeader(ack,0)
//  double timeStamp=0;
  _prot._txMsg.buf = msg->buf;
  _prot._txMsg.cmd = msg->cmd;
  
  unsigned int it_len = msg->len; // characters remaining of original msg to be sent out
  
  do {
	  printf("Yet to send: [%d]\n", it_len);

    // send the TX header (cmd, len)
    if(it_len > _prot.PROT_MAX_PAYLOAD) {
      _prot._txMsg.len=_prot.PROT_MAX_PAYLOAD+1; // +1 means more to come after this packet
      _prot.putHeader(&_prot._txMsg);
      _prot._txMsg.len-=1; // prepare for content delivery of exact number of characters
    } else {
      _prot._txMsg.len=it_len;
      _prot.putHeader(&_prot._txMsg);
    }
    printf("\theader sent\n");
    
    // send the TX payload (if any)
    if(_prot._txMsg.len > 0) {
      _prot.putContent(&_prot._txMsg); // blocking
    }
    printf("\tcontent[%d] sent\n", _prot._txMsg.len);
    // shift the buffer pointer appropriately
    _prot._txMsg.buf = &(_prot._txMsg.buf[_prot._txMsg.len]);
    
    // wait for the TX ack from PC
    if(useAcks) {
      do{
        if(_prot.timeoutHeaderTest()){
          return;
        }
        _prot.getHeader(&_prot._ackMsg);
        if(_prot._ackMsg.cmd=='L')
          _prot.listenPending = true;
        
      } while(_prot._ackMsg.cmd=='L');
      
      if(_prot._ackMsg.cmd != msg->cmd) {
        // handle exception when ACK!=CMD or ACKLEN!=0
        msg->buf[0]='\0';
        _prot.error("bad ACK in TxProtHandler");
        while(1); // die here for ease of debug
        //defaultInit();
      }
    }
    else {
      // if the host acks and we aren't expecting acks, then flush the RX channel to prevent overflow
      //        	uart_RxChanFlush(); // happens on each iteration here
      /* JTE: in Orion we are seeing data loss for large posted payloads
      * ading a delay between each packet here to mitigate potential for fifo overflow in the mbed controller
      */
#if 0
      volatile uint32_t start = GET_TIME();
      volatile uint32_t current = GET_TIME();
      
      while(current - start < 2){
        // delay ~0.97ms ms between packets
        current = GET_TIME();
      }
#endif
    }
    
    it_len -= _prot._txMsg.len;
  } while (it_len > 0);
  
  // when done transmitting without ACKs, flush the rx channel once more in case an ACK was sent AFTER we last flushed it
  if(!useAcks){
    //    	uart_RxChanFlush();
  }
}

// void c_protocol_TxProtHandler(msg_t *msg){
// _prot.TxProtHandler(msg, true);
// }

void c_protocol_error(char *dmsg){
  msg_t emsg;
  emsg.cmd='!';
  emsg.buf=dmsg;
  
  if(dmsg)
    emsg.len=strlen(dmsg);
  else
    emsg.len=0;
  
  _prot.putHeader(&emsg);
  _prot.putContent(&emsg);
  _prot._comms->resync(); // attempt to resync the comms channel
}
