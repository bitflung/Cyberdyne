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


extern uint32_t *getCamData(void);
extern int image_processing_phase1();
extern int image_processing_phase2();

C_ADUP *adup;

// NOTE: I've set the ADUP internal buffer (UART circular buffer) such that the max safe packet size is 128d (0x80)
msg_t bigmsg;
//#define MSGBUF_SZ 16*1024*2+3 // 16KB, doubled for ASCII text variant to 32KB, represents the max possible msg size
#define MSGBUF_SZ (1*8*1024)+3 // couldn't fit even 16KB without stack collision; knocked it down to 8KB; will require sending 4 msgs
uint8_t msgbuf[MSGBUF_SZ];

void loop(void);
void adup_app_handler(msg_t *msg);

extern void adup_pc_handler(msg_t *msg);

void run_demo(int *image, int *voice, bool *shoot, bool *cheat);
void camera_capture(uint32_t **buf, uint32_t *sz);
void ascii_capture(uint8_t *buf, int *sz);

extern bool btn1_pressed_adup; // sticky once set by pb1 callback
extern bool btn0_capture_image;
extern bool btn0_pressed_ai;
bool adup_init_done=false;

void run_demo(int *image, int *voice, bool *shoot, bool *cheat){
	printf("running the demo...\n");
	btn0_capture_image=true;

	//image_processing_phase1();
	//image_processing_phase2();
	*image = 45; // 45% confident you are human based on image
	*voice = 78; // 78% conf you are human based on voice
	*shoot = true; // firmware decided to shoot you
	*cheat = false; // you used the konami code to survive anyway!!

	printf("\tran run_demo()\n");
	return;
}

void camera_capture(uint32_t **buf, uint32_t *sz){
	uint32_t * dat = getCamData(); //input_0; //[IMAGE_SIZE_X * IMAGE_SIZE_Y] // uint32_t[128*128] == 64KB
	*sz=128*128*4;

	*buf = dat;

	printf("shared most recent cam_capture result\n");
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
	adup_pc_handler(msg);
}

void main_adup_setup(void){
	bigmsg.bsize=MSGBUF_SZ;
	bigmsg.buf=msgbuf;
	bigmsg.len=0;

	printf("this is the console port (press return 3 times in ADUP term to sync)\n");

	adup = C_ADUP_init(230400); // passed in baud rate not yet resepcted by MAXIM drivers
	adup->setMsg(&bigmsg);
	adup->reg_callback(adup_app_handler);

	printf("ADUP init done\n");
	adup_init_done=true;
}

void main_adup_loop(void){
	if(btn1_pressed_adup){
		if(!adup_init_done){
			main_adup_setup();
		}
		adup->conditionalListen();
	}
}

void main_adup_background(void){
	// insert system management code that should run once per loop
	// e.g. handling wifi stack
}

int main_adup_main(void){
	main_adup_setup();

	while(1){
		main_adup_loop();
		main_adup_background();
	}
}

