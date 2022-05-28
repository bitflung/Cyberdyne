#ifndef C_ADUP_HANDLER_H
#define C_ADUP_HANDLER_H

#include "c_adup_lib.h"

#include "c_adup_protocol.h"

#define ADUP_H_LINE_BUF_SZ 12000u

typedef struct C_HANDLER C_HANDLER;
struct C_HANDLER {
	// functions
	void (*commonInit)(void);
	bool (*listen)(bool single);
	bool (*checkFifo)(void);

	void (*autobaudStart)(void);
	uint32_t (*autobaudTest)(void);
	void (*autobaudEnd)(void);

	bool (*getListenPend)(void);
	void (*clearListenPend)(void);
	bool (*getDeadComms)(void);
	void (*clearDeadComms)(void);

	void (*setBaud)(int baud);
	void (*quit)(void);
	void (*update)(void);
	void (*echo)(void);
	// void (*error)(void);
	void (*error)(char *dmsg);


	void (*reg_callback)(void (*f)(msg_t *msg));
	void (*RX)(msg_t *msg);
	void (*TX)(msg_t *msg);
	void (*POST)(msg_t *msg);

	msg_t* (*getMsg)(void); // returns the internal msg struct
	void (*setMsg)(msg_t* msg); // sets the internal msg struct; use this if msg is created outside of ADUP library (pass bsize=0 in constructor)

	// fields
	C_HANDLER *this;
	C_PROTOCOL *_prot;
	void (*_cb)(msg_t *msg);
	msg_t *_msg;
};


C_HANDLER* C_HANDLER_init(int baud);

/* ----- */
		
void c_handler_commonInit(void);
bool c_handler_listen(bool single);
bool c_handler_checkFifo(void);

void c_handler_autobaudStart(void);
uint32_t c_handler_autobaudTest(void);
void c_handler_autobaudEnd(void);

bool c_handler_getListenPend(void);
void c_handler_clearListenPend(void);
bool c_handler_getDeadComms(void);
void c_handler_clearDeadComms(void);

void c_handler_setBaud(int baud);
void c_handler_quit(void);
void c_handler_update(void);
void c_handler_echo(void);
//void c_handler_error(void);
void c_handler_error(char *dmsg);


void c_handler_reg_callback(void (*f)(msg_t *msg));
void c_handler_RX(msg_t *msg);
void c_handler_TX(msg_t *msg);
void c_handler_POST(msg_t *msg);

msg_t* c_handler_getMsg(void); // returns the internal msg struct
void c_handler_setMsg(msg_t* msg); // sets the internal msg struct; use this if msg is created outside of ADUP library (pass bsize=0 in constructor)


// ADUP_PROTOCOL _prot;
// void (*_cb)(msg_t *msg);
// msg_t *_msg;


#endif
