#ifndef C_ADUP_PROTOCOL_H
#define C_ADUP_PROTOCOL_H

#include "c_adup_lib.h"

#include "c_adup_comms.h"
#include "c_adup_msg_t.h"

#define DEFAULT_PROT_MAX_PAYLOAD 8

// #define USE_COMMS_TIMEOUTS

#ifdef USE_COMMS_TIMEOUTS
#define ACK_TIMEOUT_MS 5000
#endif

typedef struct C_PROTOCOL C_PROTOCOL;
struct C_PROTOCOL {
	// functions
	bool (*timeoutTest)(uint32_t expectedSize);
	bool (*timeoutHeaderTest)(void);
	// bool (*checkFifo)(void);
	bool (*checkFifo)(uint32_t expected);

	void (*defaultInit)(void);

	void (*autobaudStart)(void);
	uint32_t (*autobaudTest)(void);
	void (*autobaudEnd)(void);

	void (*setBaud)(int baud);
	void (*protocolInit)(int newsize);

	bool (*getListenPend)(void);
	void (*clearListenPend)(void);
	bool (*getDeadComms)(void);
	void (*clearDeadComms)(void);

	void (*RxProtHandler)(msg_t *msg);
//	void (*TxProtHandler)(msg_t *msg);
	void (*TxProtHandler)(msg_t *msg, bool useAcks);
	void (*error)(char *dmsg);

	void (*getHeader)  (msg_t *msg); // get 2 chars [cmd, len]
	void (*getContent) (msg_t *msg); // get [len] characters content

	void (*putHeader)  (msg_t *msg); // put [ack, len] chars
	void (*putContent) (msg_t *msg); // put [len] characters content

	// fields
	bool listenPending;
	bool deadComms;
	C_COMMS *_comms;
	unsigned int PROT_MAX_PAYLOAD;
	msg_t _ackMsg;
	msg_t _txMsg;
};


C_PROTOCOL* C_PROTOCOL_init(int baud);

/* ----- */

// ADUP_PROTOCOL();
// ADUP_PROTOCOL(int baud);

bool c_protocol_timeoutTest(uint32_t expectedSize);
bool c_protocol_timeoutHeaderTest();
// bool checkFifo(void);
bool c_protocol_checkFifo(uint32_t expected);

void c_protocol_defaultInit();

void c_protocol_autobaudStart(void);
uint32_t c_protocol_autobaudTest(void);
void c_protocol_autobaudEnd(void);

void c_protocol_setBaud(int baud);
void c_protocol_protocolInit(int newsize);

bool c_protocol_getListenPend();
void c_protocol_clearListenPend();
bool c_protocol_getDeadComms();
void c_protocol_clearDeadComms();

void c_protocol_RxProtHandler(msg_t *msg);
// void TxProtHandler(msg_t *msg);
void c_protocol_TxProtHandler(msg_t *msg, bool useAcks);
void c_protocol_error(char *dmsg);

void c_protocol_getHeader  (msg_t *msg); // get 2 chars [cmd, len]
void c_protocol_getContent (msg_t *msg); // get [len] characters content

void c_protocol_putHeader  (msg_t *msg); // put [ack, len] chars
void c_protocol_putContent (msg_t *msg); // put [len] characters content


// bool listenPending;
// bool deadComms;
// ADUP_COMMS _comms;
// unsigned int PROT_MAX_PAYLOAD;
// msg_t _ackMsg;
// msg_t _txMsg;




#endif

