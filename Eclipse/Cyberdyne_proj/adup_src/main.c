/*******************************************************************************
 * Jared's ADUP code
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   ADUP!
 * @details ADUP test
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "dma.h"
#include "nvic_table.h"

#include "c_adup.h"


C_ADUP *adup;

// NOTE: I've set the ADUP internal buffers such that the max safe packet size is 64d (0x40)
msg_t bigmsg;
#define MSGBUF_SZ 16*1024*2+3 // 16KB, doubled for ASCII text variant to 32KB, represents the max possible msg size
uint8_t msgbuf[MSGBUF_SZ];

void loop(void);
void adup_app_handler(msg_t *msg);

extern void adup_pc_handler(msg_t *msg);

void run_demo(int *image, int *voice, bool *shoot, bool *cheat);
void camera_capture(uint8_t *buf, int *sz);
void ascii_capture(uint8_t *buf, int *sz);


void run_demo(int *image, int *voice, bool *shoot, bool *cheat){
	printf("running the demo...\n");

	*image = 45; // 45% confident you are human based on image
	*voice = 78; // 78% conf you are human based on voice
	*shoot = true; // firmware decided to shoot you
	*cheat = false; // you used the konami code to survive anyway!!

	printf("\tran run_demo()\n");
	return;
}

void camera_capture(uint8_t *buf, int *sz){
	int size=128*128;

	// shovel camera data into the buffer
	for(int i=0; i<size; i++){
		buf[i]='F';
	}

	*sz = size;

	printf("ran camera_capture()\n");
	return;
}

void ascii_capture(uint8_t *buf, int *sz){
	int size=16*16;

	// shovel ascii art data into the buffer
	for(int i=0; i<size; i++){
		buf[i]=(0xFF & i);
	}

	*sz = size;

	printf("ran ascii_capture()\n");
	return;
}

void adup_app_handler(msg_t *msg){
	// this handler to be registered as a callback in ADUP and automatically called when we RX a msg that isn't handled by the stack itself
	//if(msg->cmd!='A') adup->error("Unsupported command prefix");
//	switch(msg->cmd){
//	default:
//		adup->error("Unsupported command prefix");
//		break;
//	}
//	adup->TX(msg);
	adup_pc_handler(msg);
}

void setup(void){
	bigmsg.bsize=MSGBUF_SZ;
	bigmsg.buf=msgbuf;
	bigmsg.len=0;

	printf("this is the console port (press return 3 times in ADUP term to sync)\n");

	adup = C_ADUP_init(115200); // passed in baud rate not yet resepcted by MAXIM drivers
	adup->setMsg(&bigmsg);
	adup->reg_callback(adup_app_handler);

	printf("ADUP init done\n");
}

void loop(void){
	adup->conditionalListen();
}

void background(void){
	// insert system management code that should run once per loop
	// e.g. handling wifi stack
}

int main(void){
	setup();

	while(1){
		loop();
		background();
	}
}

