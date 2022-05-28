#ifndef C_ADUP_COMMS_H
#define C_ADUP_COMMS_H

#include "c_uart_inst.h"
#include "c_adup_lib.h"

//#include <common.h>
//#include <adi_processor.h>
//#include <sys/platform.h>
////#include "adi_initialize.h"
//#include <drivers/uart/adi_uart.h>
//#include <drivers/pwr/adi_pwr.h>
//#include <common/adi_timestamp.h>

#define TX_DELAY_FLOW_CTL false
#define TX_BYTE_CNT_PERIOD 512 // 128 bytes
#define TX_BYTE_TIM_PERIOD 100 // 25@512 = 0.8Hz //20@256 = 0.5Hz WIN-FAIL // @128 bytes: 1ms < 5ms < WORKING? < 10ms < 100ms

typedef struct C_COMMS C_COMMS;
struct C_COMMS {
	// functions
	
	bool (*checkFifo)(uint32_t expected);
	bool (*headerPeek)(void);

	void (*getString)(char* line, unsigned int len);
	void (*putString)(char* line, unsigned int len);

	void (*resync)(void);
	void (*flush)(void);

//	void (*autobaudStart)(void);
//	uint32_t (*autobaudTest)(void);
//	void (*autobaudEnd)(void);
//
//	void (*setBaud)(int baud);

	C_UART_INST *_uart;
	// fields
	uint32_t 		 		 txByteCnt;
	double					 txTimeStamp;
};

C_COMMS* C_COMMS_init(int baud);

/* ----- */


// ADUP_COMMS();
// ADUP_COMMS(bool connect);

// bool checkFifo(void); // defaults to 3
bool c_comms_checkFifo(uint32_t expected);
bool c_comms_headerPeek(void);

void c_comms_getString(char* line, unsigned int len);
void c_comms_putString(char* line, unsigned int len);

void c_comms_resync(void);
void c_comms_flush(void);

void c_comms_autobaudStart(void);
uint32_t c_comms_autobaudTest(void);
void c_comms_autobaudEnd(void);

void c_comms_setBaud(int baud);


// uint32_t 		 		 txByteCnt;
// double					 txTimeStamp;
// ADI_UART_HANDLE 		 *_hDevice;




#endif

