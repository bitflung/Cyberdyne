#include "pc_gui.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include "mxc_device.h"
//#include "led.h"
//#include "board.h"
//#include "mxc_delay.h"
//#include "uart.h"
//#include "dma.h"
//#include "nvic_table.h"

extern C_ADUP *adup;
extern void run_demo(int *image, int *voice, bool *shoot, bool *cheat);
extern void camera_capture(uint8_t **buf, uint32_t *sz);
extern void ascii_capture(uint8_t *buf, int *sz);

//static uint8_t cam_data[128*128];

void adup_pc_handler(msg_t *msg){
	switch(msg->cmd){
	case('R'):
		run_handler(msg);
		break;
	case('C'):
		cam_handler(msg);
		break;
	case('A'):
		ascii_handler(msg);
		break;
	default:
		adup->error("Unsupported command prefix");
		break;
	}

	adup->TX(msg);
}

void run_handler(msg_t *msg){
	// handle a run command

	// REQ: there is no payload

	// PROCESS:
	int image, voice;
	bool shoot, cheat;
	run_demo(&image, &voice, &shoot, &cheat);

	// RES:
	sprintf(msg->buf, "%03d%03d%01d%01d", image, voice, shoot, cheat);

	msg->len = strlen(msg->buf);
}

void cam_handler(msg_t *msg){
	// handle a camera command
	uint8_t tmp[9];
	uint32_t *cam_data;
	uint32_t sz; // size of data in BYTES, not HEX CHARS

	// REQ: there is no payload
	printf("inside cam_handler\n");

	// PROCESS:
	camera_capture(&cam_data, &sz);
	printf("captured real cam data\n");

	// RES:
	int numMsgs = sz/(1024*4); // 64KB data -> 128KB hex ascii -> div(4KB data per packet aka 8KB hex) = 16 packets


	// -- tell the GUI how many data packets we will POST
	sprintf(msg->buf, "%X", numMsgs);
	msg->len = strlen(msg->buf);
	adup->TX(msg);

	uint32_t offset = sz/(numMsgs*4);
	printf("sending [%d] (data)bytes in [%d] msgs, striding by [%d bytes] between msgs\n", sz, numMsgs, offset);

	uint32_t *alias[numMsgs];

	for(int i=0; i<numMsgs; i++){
		// create a pointer to each 8KB segment of the data
		alias[i]=(&cam_data[offset*i]);
	}

	printf("about to send %d large msgs, please be patient\n", numMsgs);
	uint32_t *a;
	for(int i=0; i<numMsgs; i++){
		a=alias[i];
		sprintf(msg->buf, "");
		for(int j=0; j<offset/4; j++){
			// TODO: note that we assume sz is evenly divisible by 4!!
			sprintf(tmp, "%08X", a[j]);
			strcat(msg->buf, tmp);
		}
		msg->len = strlen(msg->buf);
		adup->POST(msg);
		printf("%d\n", i);
	}
	printf("\nsent\n");
	sprintf(msg->buf, "DONE");
	msg->len = 4;
}

void ascii_handler(msg_t *msg){
	// handle a camera command

	// REQ: there is no payload

	// PROCESS:
	int sz;
	ascii_capture(msg->buf, &sz);

	// RES:
	msg->len = (sz>0)?sz:(16*16); // if firmware says size is negative then use default size of 128x128
}
