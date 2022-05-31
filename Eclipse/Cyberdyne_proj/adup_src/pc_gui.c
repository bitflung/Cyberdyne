#include "pc_gui.h"
#include <stdio.h>
#include <stdlib.h>
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

extern char *imageResults;//[40];
extern char *voiceResults;
extern char *decisionResults;

//static uint8_t cam_data[128*128];

void cmd_camera(msg_t *msg){
	/* payload indicates subcmd:
	 * 0: capture image
	 * 1: classify existing image buffer
	 * 2: transfer image buffer to pc
	*/

	msg_t dmsg;
	char dbuf[]="About to run image classifier\n";
	dmsg.bsize=strlen(dbuf);
	dmsg.cmd='D';
	dmsg.len=dmsg.bsize;
	dmsg.buf=dbuf;


	switch(msg->buf[0]){
	case('0'):
		// capture image data
		image_processing_phase1();
		strcpy(msg->buf, "OK");
		msg->len = strlen(msg->buf);
		printf("captured camera image\n");
		break;
	case('1'):
		// classify current image buffer
		//printf("transmitting a debug msg\n");
		adup->POST(&dmsg); // post a debug msg
		image_processing_phase2();
		strcpy(msg->buf, "OK");
		msg->len = strlen(msg->buf);
		printf("classified image\n");
		break;
	case('2'):
		// transfer image data to PC
		cam_xfer_pc(msg);
		sprintf(msg->buf, "OK");
		msg->len = strlen(msg->buf);
		printf("transferred image to GUI\n");
		break;
	default:
		// invalid subcmd
		adup->error("Invalid camera subcmd");
		break;
	}
	//cam_handler(msg);
}

void cmd_voice(msg_t *msg){
	// payload indicates how many attempts to allow while checking for keyword match
	int numTries = strtol(msg->buf, NULL, 16);

	sprintf(msg->buf, "will accept up to [%d] attempts to be recognized\n", numTries);
	msg->len=strlen(msg->buf);
	adup->TX(msg);

	start_voice_recog(numTries);
	msg->len=0;
}

void cmd_results(msg_t *msg){
	/*
	 * subcmds
	 * 0: % human img
	 * 1: % human voice
	 * 2: decision (kill/mokill)
	 * 3: ascii art
	 * 4: cheated?
	 */
	switch(msg->buf[0]){
	case('0'):
		sprintf(msg->buf, imageResults);
		msg->len = strlen(msg->buf);
		break;
	case('1'):
		sprintf(msg->buf, voiceResults);
		msg->len = strlen(msg->buf);
		break;
	case('2'):
		sprintf(msg->buf, decisionResults);
		msg->len = strlen(msg->buf);
		break;
	default:
		adup->error("unsupported results subcmd");
		break;
	}
}

void cmd_transfer(msg_t *msg){
	// we receive an image from the PC and store it in the image buffer
	// it can be classified later...
	// it MIGHT be a patched image

	// get the number of msgs pending from initial msg payload
	int numMsgs=0;
	numMsgs = strtol(msg->buf, NULL, 16);
	msg->len=0;
	printf("expecting [%d] msgs\n", numMsgs);
	adup->TX(msg); // PC will wait for a response here so we don't overwrite the numMsgs payload; we fire back the same msg
	char tmp[9];
	tmp[8]='\0';

	// NOTE: we assume msgs are sent as an integer number of words (32-bit, as 8 char ascii hex vals)
	uint32_t *dest = getCamData(); // pointer to image data array
	uint32_t idx=0;
	uint32_t val;
	for(int i=0; i<numMsgs; i++){
		adup->RX(msg);
		printf("RX'd img msg [%d] of [%d]\n", i, numMsgs-1);
		for(int j=0; j<msg->len; j+=8){
			// striding by 8 on each iteration for 8 chars -> 4 bytes of data
			for(int k=0; k<8; k++){
				tmp[k]=msg->buf[j+k];
			}

			val = strtol(tmp, NULL, 16);//&msg->buf[j], msg->buf[j+8], 16);
			dest[idx]=val;

			if(idx < 4){
				printf("word [%d]: tmp=[%s] val=[%08X] dest[%d]=[%08X]\n", idx, tmp, val, idx, dest[idx]);
			}

			idx++;
		}
		sprintf(msg->buf, "%d", i);
		msg->len = strlen(msg->buf);
		adup->TX(msg);
	}

	sprintf(msg->buf, "OK");
	msg->len = strlen(msg->buf);
}

void adup_pc_handler(msg_t *msg){
	printf("called adup_pc_handler\n");
	switch(msg->cmd){
//	case('R'):
//		run_handler(msg);
//		break;
	case('R'):
		cmd_results(msg);
		break;
	case('C'):
		cmd_camera(msg);
		break;
	case('V'):
		cmd_voice(msg);
		break;
	case('T'):
		cmd_transfer(msg);
		break;
//	case('A'):
//		ascii_handler(msg);
//		break;
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

void cam_xfer_pc(msg_t *msg){
	// handle a camera command
	uint8_t tmp[9];
	uint32_t *cam_data;
	uint32_t sz; // size of data in BYTES, not HEX CHARS

	// REQ: there is no payload
	printf("inside cam_handler\n");

	// PROCESS:
	cam_data = getCamData(); //input_0; //[IMAGE_SIZE_X * IMAGE_SIZE_Y] // uint32_t[128*128] == 64KB
	sz=128*128*4;
	printf("captured real cam data\n");

	// RES:
	int numMsgs = sz/(1024*4); // 64KB data -> 128KB hex ascii -> div(4KB data per packet aka 8KB hex) = 16 packets


	// -- tell the GUI how many data packets we will POST
	sprintf(msg->buf, "%X", numMsgs);
	msg->len = strlen(msg->buf);
	adup->TX(msg);

	uint32_t offset = sz/(numMsgs); // numMsgs/4?
	printf("sending [%d] (data)bytes in [%d] msgs, striding by [%d bytes] between msgs\n", sz, numMsgs, offset);

	uint32_t *alias[numMsgs];

	for(int i=0; i<numMsgs; i++){
		// create a pointer to each 8KB segment of the data
		alias[i]=(&cam_data[(offset*i)>>2]);
	}

	printf("about to send %d large msgs, please be patient\n", numMsgs);
	uint32_t *a;
	for(int i=0; i<numMsgs; i++){
		a=alias[i];
		sprintf(msg->buf, "");
		printf("next chunk at adr [0x%08X]\n", (uint32_t)(a));
		printf("processing [offset=%d] bytes as 4-byte words\n", offset);
		for(int j=0; j<offset/4; j++){
			// TODO: note that we assume sz is evenly divisible by 4!!
			sprintf(tmp, "%08X", a[j]);
			strcat(msg->buf, tmp);
		}
		msg->len = strlen(msg->buf);
		adup->POST(msg);
		printf("%d: POSTed [%d] hex chars\n", i, msg->len);
	}
	printf("\nsent\n");
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
