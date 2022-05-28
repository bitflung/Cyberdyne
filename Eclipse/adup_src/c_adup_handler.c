#include "c_adup_handler.h"

C_HANDLER _handler;
msg_t _default_msg;

C_HANDLER* C_HANDLER_init(int baud){
  // functions
  _handler.commonInit = &c_handler_commonInit;
  _handler.listen = &c_handler_listen;
  _handler.checkFifo = &c_handler_checkFifo;
  
  _handler.autobaudStart = &c_handler_autobaudStart;
  _handler.autobaudTest = &c_handler_autobaudTest;
  _handler.autobaudEnd = &c_handler_autobaudEnd;
  
  _handler.getListenPend = &c_handler_getListenPend;
  _handler.clearListenPend = &c_handler_clearListenPend;
  _handler.getDeadComms = &c_handler_getDeadComms;
  _handler.clearDeadComms = &c_handler_clearDeadComms;
  
  _handler.setBaud = &c_handler_setBaud;
  _handler.quit = &c_handler_quit;
  _handler.update = &c_handler_update;
  _handler.echo = &c_handler_echo;
  // error = &c_handler_error;
  _handler.error = &c_handler_error;
  
  
  _handler.reg_callback = &c_handler_reg_callback;
  _handler.RX = &c_handler_RX;
  _handler.TX = &c_handler_TX;
  _handler.POST = &c_handler_POST;
  
  _handler.getMsg = &c_handler_getMsg;
  _handler.setMsg = &c_handler_setMsg;
  
  // fields
  // _handler.this = _handler;
  _handler._prot = C_PROTOCOL_init(baud);
  _handler._cb = NULL;
  _handler._msg = &_default_msg;
  
  return &_handler;
}

void c_handler_commonInit(void){
  // _handler._msg = &_default_msg;
  
  _handler._msg->buf=NULL;
  _handler._msg->bsize=0;
  _handler._msg->cmd='!';
  _handler._msg->len=0;
  
  // _handler._cb=NULL;
}

// c_handler_ADUP_HANDLER(int bufsize) : _prot(){
// _handler.commonInit(bufsize);
// }

// c_handler_ADUP_HANDLER(int bufsize, int baud) : _prot(baud){
// _handler.commonInit(bufsize);
// }

bool c_handler_listen(bool single){
  bool done=single;
  // receive a command (1 char), length (2 chars), and payload (large string buffer)
  //printf("\twaiting to read a command\n");
  
  _handler.RX(_handler._msg);
  
  // exit early if we've got a dead comms channel
  if(_handler.getDeadComms())
    return true; // return value of true means we are DONE looping
  
  //printf("\tgot a command; parsing it now...\n");
  switch(_handler._msg->cmd) {
    case('L'): // LISTEN (tells the MCU to enter into a LISTEN loop
      done = false;
      break;
  case ('B'): // BAUD: partially implemented function to set baud rate via commands
    //_handler.setBaud(); // update the baud rate: only partially implemented!
    break;
  case ('Q'): // QUIT: ends the internal LISTEN loop
    //_handler.quit();
    done=true; // tell whomever called on listener() that it should stop looping
    break;
  case ('U'): // UPDATE: integral control over packet sizes
    _handler.update(); // update the max packet length
    break;
  case ('#'): // ECHO: tells MCU to echo back the full msg as received
    _handler.echo(); // return the exact same payload
    break;
  default: // FALL THRU: calls the registered callback, if one exists
    if(_handler._cb == NULL) {
      // Default for empty handler: do nothing
      _handler.error("parsed a non-default msg without a registered callback to handle it");
    }
    else {
      _handler._cb(_handler._msg); // else call on the callback function
    }
    break;
  }

  return done;
}

bool c_handler_checkFifo(void){
  return _handler._prot->checkFifo(3); // was void, made 3
}

void c_handler_autobaudStart(void){
  _handler._prot->autobaudStart();
}

uint32_t c_handler_autobaudTest(void){
  return _handler._prot->autobaudTest();
}

void c_handler_autobaudEnd(void){
  _handler._prot->autobaudEnd();
}

void c_handler_setBaud(int baud){
  //    _msg->buf[_msg->len]='\0'; // sanitize the input
  //    int tmpInt = strtol(_msg->buf, NULL, 16);
  _handler._prot->setBaud(baud); // >0 set to value; 0 calls autobaud; -1 uses uart_config.h
  return;
}

bool c_handler_getListenPend(){
  return _handler._prot->getListenPend();
}

void c_handler_clearListenPend(){
  _handler._prot->clearListenPend();
}

bool c_handler_getDeadComms(){
  return _handler._prot->getDeadComms();
}

void c_handler_clearDeadComms(){
  _handler._prot->clearDeadComms();
}

void c_handler_quit(){
  // Q.uit: handled in the case statement above; this quits the handler loop
  return;
}

void c_handler_update(){    
  // U.pdate: updates the max payload size
  unsigned int tmpInt;
  _handler._msg->buf[2]='\0'; // ensure we only have 2 characters of payload size here...
  tmpInt=strtol(_handler._msg->buf, NULL, 16);
  if(tmpInt+3 < _handler._msg->bsize){
    _handler._prot->protocolInit(tmpInt); // negotiate max payload size
  }
  else
    _handler.error("bad update");
}

void c_handler_echo(){
  _handler._prot->TxProtHandler(_handler._msg, true); // reply with the same values we received
}

void c_handler_error(char *dmsg){
  _handler._prot->error(dmsg);
  //while(1); // die here for ease of debug
}

// void c_handler_error(){
// _handler._prot->error(NULL);
// /*while(1); // die here for ease of debug*/
// }

void c_handler_reg_callback(void (*f)(msg_t *msg)){
  _handler._cb=f;
}

void c_handler_RX(msg_t *msg){
  _handler._prot->RxProtHandler(msg);
}

void c_handler_TX(msg_t *msg){
  _handler._prot->TxProtHandler(msg, true);
}

void c_handler_POST(msg_t *msg){
  _handler._prot->TxProtHandler(msg, false); // bool flag is optional and indicates whether ACKs are expected/used
}

void c_handler_setMsg(msg_t *msg){
  _handler._msg = msg;
}

msg_t* c_handler_getMsg(){
  return _handler._msg;
}
