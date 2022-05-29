/*******************************************************************************
 * Copyright (C) 2020-2021 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************/

// ------------------
// JTE: adding for ADUP flow
#include "board.h"
#include "pc_gui.h"
#include "c_adup.h"
#include "pb.h"

void main_adup_loop(void);
extern void main_adup_setup(void);

extern void ai_loop(void);
extern int ai_setup(void);

bool btn0_pressed_ai;
bool btn0_capture_image;
bool btn1_pressed_adup; // sticky once set to true; eventually we can fire a command from the PC to disconnect?

#define ADUP_ON_BY_DEFAULT true

uint32_t resultsSize = 40;
char *imageResults;//[40];
char *voiceResults;//[40];
char *decisionResults;//[40];
bool cheatCode=false;

// -------------------
void cb_PB_0(void *v){
	printf("RX'd PB0 event\n");
	if(btn1_pressed_adup){
		printf("demo controlled via GUI now\n");
	}
	else{
		btn0_capture_image=true;
		btn0_pressed_ai=true;
	}
}

void cb_PB_1(void *v){
	printf("RX'd PB1event\n");
	btn1_pressed_adup=true;
}

void local_setup(void){
	resultsSize = 40;
	imageResults = malloc(sizeof(char)*resultsSize);//[40];
	voiceResults = malloc(sizeof(char)*resultsSize);//[40];
	decisionResults = malloc(sizeof(char)*resultsSize);//[40];

	snprintf(imageResults, 40, "Not yet tested");
	snprintf(voiceResults, 40, "Not yet tested");
	snprintf(decisionResults, 40, "Not yet tested");
	cheatCode=false;

	btn0_pressed_ai=false;
	btn1_pressed_adup=ADUP_ON_BY_DEFAULT;

	PB_Init();
	PB_RegisterCallback(0, cb_PB_0);
	PB_RegisterCallback(1, cb_PB_1);
}

void setup(void){
	// Console_Shutdown(); // kill the console UART; printf() wont print anywhere
	local_setup();
	//main_adup_setup(); // called by main_adup_loop after pressing pb1
	ai_setup();
}

void loop(void){
	main_adup_loop();
	ai_loop();
}

int main(void){
	setup();
	while(1) loop();
	return 0;
}
