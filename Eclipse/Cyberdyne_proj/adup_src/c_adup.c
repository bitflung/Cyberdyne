
#include "c_adup.h"

C_ADUP _adup;

C_ADUP* C_ADUP_init(int baud){
	_adup.getDeadComms = &c_adup_getDeadComms;
	_adup.clearDeadComms = &c_adup_clearDeadComms;

	_adup.conditionalListen = &c_adup_conditionalListen;
	_adup.listen = &c_adup_listen;
	_adup.reg_callback = &c_adup_reg_callback;
	_adup.error = &c_adup_error;
	_adup.RX = &c_adup_RX;
	_adup.TX = &c_adup_TX;
	_adup.POST = &c_adup_POST;

	_adup.autobaudStart = &c_adup_autobaudStart;
	_adup.autobaudTest = &c_adup_autobaudTest;
	_adup.autobaudEnd = &c_adup_autobaudEnd;

	_adup.setBaud = &c_adup_setBaud;
	_adup.getMsg = &c_adup_getMsg;
	_adup.setMsg = &c_adup_setMsg;

	// _adup.this = _adup;
	_adup._handler = C_HANDLER_init(baud);
	
	return &_adup;	
}

// checks fifo and calls listen ONLY if the RX fifo contains 3 or more bytes
// calls on the listen function ONCE (to ack this header) and then iterates in LISTEN loop if and only if this header was a 'L'isten header
void c_adup_conditionalListen(){
	if(_adup._handler->getListenPend()){
		// if we RX'd a listen command in the middle of some other TX operation, then

		_adup._handler->clearListenPend(); // clear the listenPend
		
		msg_t listenAck;
		listenAck.cmd='L';
		listenAck.len=0;
		_adup.POST(&listenAck);
//		_adup.TX(&listenAck); // ACK the pending listen command

		_adup.listen(false); // and start listening... param(false) -> the listen loop isn't 'done' on the first pass
	}
	else{
		// we haven't received a listen command, so

		// check if the fifo has data for us to consume
		if(_adup._handler->checkFifo()){
			// is so, consume that data. it MIGHT be a listen command
			_adup.listen(true); // param(true) -> single cycle listen call UNLESS received header is 'L'isten
		}
	}
}


bool c_adup_getDeadComms(){
	return _adup._handler->getDeadComms();
}

void c_adup_clearDeadComms(){
	_adup._handler->clearDeadComms();
}


// blocks until a 'Q'uit command is received from the PC side
void c_adup_listen(bool single){
	bool done=single;

	if(_adup.getDeadComms())
		return; // don't listen if the comms channel is dead

	do{
    	//printf("calling handler.listen()\n");
        done=_adup._handler->listen(done); // calls on our callback by itself
    } while(!done);

	// what if we exited out due to dead comms?
	// app layer should do something to stop trying to talk to the remote device
}

void c_adup_autobaudStart(void){
	_adup._handler->autobaudStart();
}

uint32_t c_adup_autobaudTest(void){
	return _adup._handler->autobaudTest();
}

void c_adup_autobaudEnd(void){
	_adup._handler->autobaudEnd();
}

void c_adup_setBaud(int baud){
	_adup._handler->setBaud(baud);
}

void c_adup_reg_callback(void (*f)(msg_t *msg)){
    _adup._handler->reg_callback(f);
}

void c_adup_error(char *dmsg){
	if(_adup.getDeadComms())
		return;

    _adup._handler->error(dmsg);
}

void c_adup_RX(msg_t *msg){
	if(_adup.getDeadComms())
		return;

    _adup._handler->RX(msg);
}

void c_adup_TX(msg_t *msg){
	if(_adup.getDeadComms())
		return;

    _adup._handler->TX(msg);
}

void c_adup_POST(msg_t *msg){
	if(_adup.getDeadComms())
		return;

    _adup._handler->POST(msg);
}

void c_adup_setMsg(msg_t *msg){
	_adup._handler->setMsg(msg);
}

msg_t* c_adup_getMsg(){
	return _adup._handler->getMsg();
}
