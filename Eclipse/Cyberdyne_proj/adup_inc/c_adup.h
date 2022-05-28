#ifndef C_ADUP_H
#define C_ADUP_H

#include "c_adup_lib.h"
#include "c_adup_handler.h"

typedef struct C_ADUP C_ADUP;
struct C_ADUP {
  // functions
  bool (*getDeadComms)(void);
  void (*clearDeadComms)(void);
  
  void (*conditionalListen)(void);
  void (*listen)(bool single);
  void (*reg_callback)(void (*f)(msg_t *msg));
  void (*error)(char *dmsg);
  void (*RX)(msg_t *msg);
  void (*TX)(msg_t *msg);
  void (*POST)(msg_t *msg);
  
  void (*autobaudStart)(void);
  uint32_t (*autobaudTest)(void);
  void (*autobaudEnd)(void);
  
  void (*setBaud)(int baud);
  msg_t* (*getMsg)(void); // returns the internal msg struct
  void (*setMsg)(msg_t* msg); // sets the internal msg struct; use this if msg is created outside of ADUP library (pass bsize=0 in constructor)
  
  // fields
  C_ADUP *this;
  C_HANDLER *_handler;
  
};

C_ADUP* C_ADUP_init(int baud);

/* ----- */

bool c_adup_getDeadComms();
void c_adup_clearDeadComms();

void c_adup_conditionalListen();
void c_adup_listen(bool single);
void c_adup_reg_callback(void (*f)(msg_t *msg));
void c_adup_error(char *dmsg);
void c_adup_RX(msg_t *msg);
void c_adup_TX(msg_t *msg);
void c_adup_POST(msg_t *msg);

void c_adup_autobaudStart(void);
uint32_t c_adup_autobaudTest(void);
void c_adup_autobaudEnd(void);

void c_adup_setBaud(int baud);
msg_t* c_adup_getMsg(); // returns the internal msg struct
void c_adup_setMsg(msg_t* msg); // sets the internal msg struct; use this if msg is created outside of ADUP library (pass bsize=0 in constructor)

// ADUP_HANDLER _handler;


#endif
