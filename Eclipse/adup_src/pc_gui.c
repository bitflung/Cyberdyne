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
extern void camera_capture(uint8_t *buf, int *sz);
extern void ascii_capture(uint8_t *buf, int *sz);

static uint8_t cam_data[128*128];

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


	// REQ: there is no payload
	printf("inside cam_handler\n");

	// PROCESS:
	int sz;
	camera_capture(cam_data, &sz);
	printf("captured fake cam data\n");

	// RES:
	if(sz*2 > msg->bsize) {
		printf("whoa boy, no room for this msg, making it barely fit");

		sz=msg->bsize;
		sz/=2;
	}

	sprintf(msg->buf, "");
	uint32_t *alias = ((uint32_t *)(cam_data));
	for(int i=0; i<sz/4; i++){
		// TODO: note that we assume sz is evenly divisible by 4!!
		sprintf(tmp, "%08X", alias[i]);
		strcat(msg->buf, tmp);
	}

#if 1
	msg->len = strlen(msg->buf);
	adup->POST(msg);
	sprintf(msg->buf, "DONE");
	msg->len = 4;

#else
	uint8_t *orig_buf = msg->buf;
	int origLen = strlen(msg->buf);
	int curLen = origLen;
	int step = 128;
	int idx=0;
	printf("sending cam response in a bunch of packets\n");
	msg->len = step;
	while(curLen > step){
		printf("cam send idx[%d]\n", idx);
		msg->buf = &(msg->buf[idx]);
		adup->TX(msg);
		idx+=step;
		curLen-=step;
	}
	if(curLen > 0){
		printf("cam send idx[%d]\n", idx);
		msg->len = curLen;
		msg->buf = &(msg->buf[idx]);
		adup->TX(msg);
		curLen-=msg->len;
	}

	sprintf(msg->buf, "DONE");
	msg->len = strlen(msg->buf);
#endif
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
