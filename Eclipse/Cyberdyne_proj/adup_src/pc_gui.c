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
	if((sz*2)/4 > msg->bsize) {
		printf("whoa boy, no room for this msg, making it barely fit");

		sz=msg->bsize;
		sz/=2;
	}


	uint32_t offset = sz/4;
	uint32_t *alias[4];

	for(int i=0; i<4; i++){
		alias[i]=((uint32_t *)(&cam_data[offset*i]));
	}

	printf("about to send 4 large msgs, please be patient\n");
	uint32_t *a;
	for(int i=0; i<4; i++){
		a=alias[i];
		sprintf(msg->buf, "");
		for(int j=0; j<offset/4; j++){
			// TODO: note that we assume sz is evenly divisible by 4!!
			sprintf(tmp, "%08X", a[j]);
			strcat(msg->buf, tmp);
		}
		msg->len = strlen(msg->buf);
		adup->POST(msg);
		printf(".");
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
