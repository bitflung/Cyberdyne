#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc_device.h"
#include "mxc_sys.h"
#include "fcr_regs.h"
#include "icc.h"
#include "led.h"
#include "tmr.h"
#include "dma.h"
#include "pb.h"
#include "cnn_voice.h"
#include "cnn_image.h"
#include "sampledata.h"
#include "mxc_delay.h"
#include "camera.h"
#include "c_adup.h"

#ifdef BOARD_EVKIT_V1
#include "bitmap.h"
#include "tft_ssd2119.h"
#endif
#ifdef BOARD_FTHR_REVA
#include "tft_ili9341.h"
#endif

#define BOARD_FTHR_REVA

#define ASCII_TO_ADUP true

// Comment out USE_SAMPLEDATA to use Camera module
//#define USE_SAMPLEDATA

#define ASCII_ART

#define IMAGE_SIZE_X  (64*2)
#define IMAGE_SIZE_Y  (64*2)

#define TFT_X_START		100
#define TFT_Y_START		50

#define CAMERA_FREQ   (5 * 1000 * 1000)

#define TFT_BUFF_SIZE   30    // TFT buffer size

#ifdef BOARD_EVKIT_V1
int image_bitmap_1 = ADI_256_bmp;
int image_bitmap_2 = logo_white_bg_darkgrey_bmp;
int font_1 = urw_gothic_12_white_bg_grey;
int font_2 = urw_gothic_13_white_bg_grey;
#endif
#ifdef BOARD_FTHR_REVA
int image_bitmap_1 = (int)& img_1_rgb565[0];
int image_bitmap_2 = (int)& logo_rgb565[0];
int font_1 = (int)& SansSerif16x16[0];
int font_2 = (int)& SansSerif16x16[0];
#endif

const char classes[CNN_NUM_IMAGE_OUTPUTS][10] = { "HUMAN", "ROBOT" };

extern char *imageResults;//[40];
extern char *voiceResults;//[40];
extern char *decisionResults;//[40];
extern bool btn1_pressed_adup;

IMAGE_t iClassifier;

// Classification layer:
static int32_t ml_image_data[CNN_NUM_IMAGE_OUTPUTS];
static q15_t ml_image_softmax[CNN_NUM_IMAGE_OUTPUTS];
extern int start_voice_recog(int);
volatile uint32_t cnn_image_time; // Stopwatch

// RGB565 buffer for TFT
uint8_t data565[IMAGE_SIZE_X * 2];

#ifdef USE_SAMPLEDATA
// Data input: HWC 3x128x128 (49152 bytes total / 16384 bytes per channel):
static const uint32_t input_0[] = SAMPLE_INPUT_0; // input data from header file
#else
static uint32_t input_0[IMAGE_SIZE_X * IMAGE_SIZE_Y]; // buffer for camera image
#endif

void cnn_valid_state_image();
static int start_image_processing();


void delay(uint32_t tick){
	while(tick > 0){
		tick--;
	}
}

extern bool btn0_pressed_ai;
extern bool btn0_capture_image;
extern bool adup_init_done;
extern C_ADUP *adup;

int image_processing_phase1();
int image_processing_phase2();
/* **************************************************************************** */

uint32_t *getCamData(void){
	return input_0;
}

#ifdef ASCII_ART

//char * brightness = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "; // standard
char *brightness = "@%#*+=-:. "; // simple
#define RATIO 2  // ratio of scaling down the image to display in ascii
void asciiart(uint8_t *img) {
	msg_t dmsg;
	char dbuf[2];
	dmsg.cmd='D';
	dmsg.bsize=1;
	dmsg.buf=dbuf;
	dmsg.len=1;

	int skip_x, skip_y;
	uint8_t r, g, b, Y;
	uint8_t *srcPtr = img;
	int l = strlen(brightness) - 1;

	skip_x = RATIO;
	skip_y = RATIO;
	for (int i = 0; i < IMAGE_SIZE_Y; i++) {
		for (int j = 0; j < IMAGE_SIZE_X; j++) {

			// 0x00bbggrr, convert to [0,255] range
			r = *srcPtr++ ^ 0x80;
			g = *(srcPtr++) ^ 0x80;
			b = *(srcPtr++) ^ 0x80;

			srcPtr++; //skip msb=0x00

			// Y = 0.2126 * r + 0.7152 * g + 0.0722 * b;
			Y = (3 * r + b + 4 * g) >> 3; // simple luminance conversion

			if ((skip_x == RATIO) && (skip_y == RATIO)){
				sprintf(dbuf, "%c", brightness[l - (Y * l / 255)]);
				if(ASCII_TO_ADUP && btn1_pressed_adup) adup->POST(&dmsg);
				else printf("%c", dbuf[0]);
			}

			skip_x++;
			if (skip_x > RATIO)
				skip_x = 1;
		}
		skip_y++;
		if (skip_y > RATIO) {
			sprintf(dbuf, "\n");
			if(ASCII_TO_ADUP && btn1_pressed_adup) adup->POST(&dmsg);
			else printf("%c", dbuf[0]);
			skip_y = 1;
		}
	}

}

#endif

/* **************************************************************************** */

void TFT_Print(char *str, int x, int y, int font, int length) {
#ifdef TFT_ENABLE
	// fonts id
	text_t text;
	text.data = str;
	text.len = length;
	MXC_TFT_PrintFont(x, y, font, &text, NULL);
#endif
}

/* **************************************************************************** */
void fail(void) {
	printf("\n*** FAIL ***\n\n");

	while (1);
}

/* **************************************************************************** */
void cnn_load_input(void) {
	int i;
	const uint32_t *in0 = input_0;

	for (i = 0; i < 16384; i++) {
		// Remove the following line if there is no risk that the source would overrun the FIFO:
		while (((*((volatile uint32_t*) 0x50000004) & 1)) != 0)
			; // Wait for FIFO 0
		*((volatile uint32_t*) 0x50000008) = *in0++; // Write FIFO 0
	}
}

/* **************************************************************************** */
#if defined USE_SAMPLEDATA && defined TFT_ENABLE
void display_sampledata(void)
{

#ifdef TFT_ENABLE
	uint32_t  w;
	uint8_t r,g,b;
	uint16_t rgb;

	int j = 0;
	uint32_t temp;

	int cnt = 0;;
	w = IMAGE_SIZE_X;

	// Get image line by line
	for (int row = 0; row < IMAGE_SIZE_Y; row++)
	{

		//LED_Toggle(LED2);
#ifdef BOARD_EVKIT_V1
		j = IMAGE_SIZE_X*2 - 2; // mirror on display
#else
		j = 0;
#endif

		for (int k= 0; k< 4*w; k+=4){

			// sample data is already in [-128,127] range, make it [0,255] for display
			temp = input_0[cnt] ^ 0x00808080;

			// data format: 0x00bbggrr
			r = temp & 0xFF;
			g = (temp >> 8) & 0xFF;
			b = (temp >> 16) & 0xFF;
			cnt++;

			// convert to RGB656 for display
			rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
			data565[j] = (rgb >> 8) & 0xFF;
			data565[j+1] = rgb & 0xFF;

#ifdef BOARD_EVKIT_V1
			j-=2; // mirror on display
#else
			j+=2;
#endif
		}

		MXC_TFT_ShowImageCameraRGB565(TFT_X_START, TFT_Y_START + row, data565, w, 1);

		LED_Toggle(LED2);
	}
#endif
}

#elif !defined USE_SAMPLEDATA
void capture_process_camera(void) {

	uint8_t *raw;
	uint32_t imgLen;
	uint32_t w, h;

	int cnt = 0;

	uint8_t r, g, b;
	uint16_t rgb;
	int j = 0;

	uint8_t *data = NULL;
	stream_stat_t *stat;

	camera_start_capture_image();

	// Get the details of the image from the camera driver.
	camera_get_image(&raw, &imgLen, &w, &h);

	// Get image line by line
	for (int row = 0; row < h; row++) {
		// Wait until camera streaming buffer is full
		while ((data = get_camera_stream_buffer()) == NULL) {
			if (camera_is_image_rcv()) {
				break;
			}
		}

		//LED_Toggle(LED2);
#ifdef BOARD_EVKIT_V1
		j = IMAGE_SIZE_X*2 - 2; // mirror on display
#else
		j = 0;
#endif
		for (int k = 0; k < 4 * w; k += 4) {

			// data format: 0x00bbggrr
			r = data[k];
			g = data[k + 1];
			b = data[k + 2];
			//skip k+3

			// change the range from [0,255] to [-128,127] and store in buffer for CNN
			input_0[cnt++] = ((b << 16) | (g << 8) | r) ^ 0x00808080;

			// convert to RGB656 for display
			rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
			data565[j] = (rgb >> 8) & 0xFF;
			data565[j + 1] = rgb & 0xFF;
#ifdef BOARD_EVKIT_V1
			j-=2; // mirror on display
#else
			j += 2;
#endif
		}
#ifdef TFT_ENABLE
		MXC_TFT_ShowImageCameraRGB565(TFT_X_START, TFT_Y_START + row, data565, w, 1);
#endif

		//LED_Toggle(LED2);
		// Release stream buffer
		release_camera_stream_buffer();
	}

	//camera_sleep(1);
	stat = get_camera_stream_statistic();

	if (stat->overflow_count > 0) {
		printf("OVERFLOW DISP = %d\n", stat->overflow_count);
		LED_On(LED2); // Turn on red LED if overflow detected
		while (1)
			;
	}

}
#endif

/* **************************************************************************** */


int ai_setup(void){

	int ret = 0;
	int dma_channel;
#if defined (BOARD_FTHR_REVA)
	// Wait for PMIC 1.8V to become available, about 180ms after power up.
	MXC_Delay(200000);
	/* Enable camera power */
	Camera_Power(POWER_ON);
	//MXC_Delay(300000);
	printf("\nWe are CYBERDYNE. Our aim is to promote robots and make them prevail. We will take a photo of yours and "
			"will ask to speak something. If we think you are Robot, you live or else...\n\n\t\t\tWelcome to CYBERDYNE\nWe Make ROBOTS LIVE!!!!!!!!!!!!!!!!!\n");
#else
	printf("\nWelcome to CYBERDYNE\nWe Make ROBOTS LIVE!!!!!!!!!!!!!!!!!\n");
#endif

	/* Enable cache */
	MXC_ICC_Enable(MXC_ICC0);

	/* Switch to 100 MHz clock */
	MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
	SystemCoreClockUpdate();

	/* Enable peripheral, enable CNN interrupt, turn on CNN clock */
	/* CNN clock: 50 MHz div 1 */
	cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK,
			MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);

	/* Configure P2.5, turn on the CNN Boost */
	cnn_boost_enable(MXC_GPIO2, MXC_GPIO_PIN_5);


#ifdef TFT_ENABLE
	/* Initialize TFT display */
	printf("Init LCD.\n");
#ifdef BOARD_EVKIT_V1
	mxc_gpio_cfg_t tft_reset_pin = {MXC_GPIO0, MXC_GPIO_PIN_19, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};
	MXC_TFT_Init(MXC_SPI0, 1, &tft_reset_pin, NULL);
	MXC_TFT_ClearScreen();
	MXC_TFT_ShowImage(0, 0, image_bitmap_1);
#endif
#ifdef BOARD_FTHR_REVA
	/* Initialize TFT display */
	MXC_TFT_Init(MXC_SPI0, 1, NULL, NULL);
	MXC_TFT_SetRotation(ROTATE_270);

	MXC_TFT_ShowImage(0, 0, image_bitmap_1);
	MXC_TFT_SetForeGroundColor(WHITE);   // set chars to white
#endif
	MXC_Delay(1000000);
#endif

	// Initialize DMA for camera interface
	MXC_DMA_Init();
	dma_channel = MXC_DMA_AcquireChannel();

	// Initialize camera.
	//	printf("Init Camera.\n");
	camera_init(CAMERA_FREQ);

	ret = camera_setup(IMAGE_SIZE_X, IMAGE_SIZE_Y, PIXFORMAT_RGB888,
			FIFO_THREE_BYTE, STREAMING_DMA, dma_channel);
	if (ret != STATUS_OK) {
		printf("Error returned from setting up camera. Error %d\n", ret);
		return -1;
	}
	// Enable CNN clock
	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);

#ifdef BOARD_EVKIT_V1
	camera_write_reg(0x11, 0x1); // set camera clock prescaller to prevent streaming overflow
#else
	camera_write_reg(0x11, 0x3); // set camera clock prescaller to prevent streaming overflow
#endif

#ifdef TFT_ENABLE
	MXC_TFT_SetPalette(image_bitmap_2);
	MXC_TFT_SetBackGroundColor(4);
	//MXC_TFT_ShowImage(1, 1, image_bitmap_2);
	memset(buff, 32, TFT_BUFF_SIZE);
	TFT_Print(buff, 55, 50, font_2, sprintf(buff, "ANALOG DEVICES"));
	TFT_Print(buff, 55, 90, font_1, sprintf(buff, "Cats-vs-Dogs Demo"));
	TFT_Print(buff, 30, 130, font_2, sprintf(buff, "PRESS PB1(SW1) TO START!"));
#endif
}

void ai_loop(void){
	if(btn0_pressed_ai && btn0_capture_image){
//		int pb0 = 0;
		printf("********** The Game Begins Now, press PB1 to start  **********\r\n");
		start_image_processing();
		start_voice_recog(10);
		MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);
	#ifdef TFT_ENABLE
		MXC_TFT_ClearScreen();
	#endif
		btn0_capture_image=false;
		btn0_pressed_ai=false;
	}
	else if(btn0_capture_image){
		printf("capturing image\n");
		image_processing_phase1();
		btn0_capture_image=false;
	}
}

int ai_main(void) {
	if(ai_setup() != 0){
		return -1;
	}

	while(1) ai_loop();
	return 0;
}


static int start_image_processing() {
	image_processing_phase1();
	image_processing_phase2();
}

int image_processing_phase1(){
	LED_Off(LED1);
	LED_Off(LED2);
#ifdef USE_SAMPLEDATA
#ifdef TFT_ENABLE
	display_sampledata();
#endif
#else
	capture_process_camera();
#endif
}

int image_processing_phase2() {
	msg_t dmsg;
	//char dbuf[]="This is a debug msg\n";
	//dmsg.bsize=strlen(dbuf);
	dmsg.cmd='D';
	//dmsg.len=dmsg.bsize;
	//dmsg.buf=dbuf;

	int i;
	int digs, tens;

	int result[CNN_NUM_IMAGE_OUTPUTS]; // = {0};

	//	char buff[TFT_BUFF_SIZE];

	cnn_valid_state_image();
	cnn_image_start();
	cnn_load_input();

	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; // SLEEPDEEP=0
	while (cnn_image_time == 0) {
		__WFI();    // Wait for CNN interrupt
	}

	// Unload CNN data
	cnn_image_unload((uint32_t*) ml_image_data);
	cnn_stop();

	// Softmax
	softmax_q17p14_q15((const q31_t*) ml_image_data, CNN_NUM_IMAGE_OUTPUTS, ml_image_softmax);
	printf("Fate will decide if you live or...\n");
	//printf("Time for CNN: %d us\n\n", cnn_image_time);
#ifdef ASCII_ART
	asciiart((uint8_t*) input_0);
#endif
	printf("Your face says you are:\n");
	int pdigs=0;
	int sel;
	for (i = 0; i < CNN_NUM_IMAGE_OUTPUTS; i++) {
		digs = (1000 * ml_image_softmax[i] + 0x4000) >> 15;
		tens = digs % 10;
		digs = digs / 10;
		result[i] = digs;
		//			printf("[%7d] -> Class %d %8s: %d.%d%%\r\n", ml_image_data[i], i,
		//					classes[i], result[i], tens);
		iClassifier.humans = digs ? i == 0 : 0;
		iClassifier.robots = digs ? i == 1 : 0;
		if(digs>pdigs){
			pdigs=digs;
			sel=i;
		}

		printf("%8s: %d.%d%%\r\n",	classes[i], result[i], tens);
	}
	//char tmp[100];
	sprintf(imageResults, "%8s: %d%%",	classes[sel], result[sel]);

	printf("\nLet's see if you sound like a ROBOT\n");

#ifdef TFT_ENABLE

	area_t  area;
	area.x = 0;
	area.y = 0;
	area.w = 320;
	area.h = TFT_Y_START-1;
	MXC_TFT_ClearArea(&area, 4);

	memset(buff, 32, TFT_BUFF_SIZE);
#endif



	if (result[0] == result[1]) {
		//			TFT_Print(buff, TFT_X_START + 10, TFT_Y_START - 30, font_1,
		//					sprintf(buff, "Unknown"));
		LED_On(LED1);
		LED_On(LED2);

	}
	else if (ml_image_data[0] > ml_image_data[1]) {
		//			TFT_Print(buff, TFT_X_START + 10, TFT_Y_START - 30, font_1,
		//					sprintf(buff, "%s (%d%%)", classes[0], result[0]));
		LED_On(LED1);
		LED_Off(LED2);

	} else {
		//			TFT_Print(buff, TFT_X_START + 10, TFT_Y_START - 30, font_1,
		//					sprintf(buff, "%s (%d%%)", classes[1], result[1]));
		LED_Off(LED1);
		LED_On(LED2);

	}


	//		memset(buff, 32, TFT_BUFF_SIZE);
	//		TFT_Print(buff, TFT_X_START + 30, TFT_Y_START + IMAGE_SIZE_Y + 10,
	//				font_1, sprintf(buff, "%dms", cnn_image_time / 1000));
	//		TFT_Print(buff, 20, TFT_Y_START + IMAGE_SIZE_Y + 35, font_2,
	//				sprintf(buff, "PRESS PB1(SW1) TO CAPTURE"));

	//#ifdef ASCII_ART
	//		asciiart((uint8_t*) input_0);
	//#endif
	//		MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

	return 0;
}

void cnn_valid_state_image(){
	/* Bring CNN state machine into consistent state */
	cnn_init_image();
	//	/* Load CNN kernels */
	cnn_load_weights_image();
	//	/* Load CNN bias */
	cnn_load_bias_image();
	//	/* Configure CNN state machine */
	cnn_configure_image();
}

