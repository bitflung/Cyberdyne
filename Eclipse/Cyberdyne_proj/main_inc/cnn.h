/**************************************************************************************************
* Copyright (C) 2019-2021 Maxim Integrated Products, Inc. All Rights Reserved.
*
* Maxim Integrated Products, Inc. Default Copyright Notice:
* https://www.maximintegrated.com/en/aboutus/legal/copyrights.html
**************************************************************************************************/

/*
 * This header file was automatically generated for the cats-dogs network from a template.
 * Please do not edit; instead, edit the template and regenerate.
 */

#ifndef __CNN_H__
#define __CNN_H__


#include "gpio_regs.h"
typedef int32_t q31_t;
typedef int16_t q15_t;

typedef struct{
	uint32_t humans;
	uint32_t robots;
}IMAGE_t;

/* Return codes */
#define CNN_FAIL 0
#define CNN_OK 1

/* Use this timer to time the inference */
#define CNN_INFERENCE_TIMER MXC_TMR0



/* Port pin actions used to signal that processing is active */

#define CNN_START LED_On(1)
#define CNN_COMPLETE LED_Off(1)
#define SYS_START LED_On(0)
#define SYS_COMPLETE LED_Off(0)

/* Run software SoftMax on unloaded data */
void softmax_q17p14_q15(const q31_t * vec_in, const uint16_t dim_vec, q15_t * p_out);
/* Shift the input, then calculate SoftMax */
void softmax_shift_q17p14_q15(q31_t * vec_in, const uint16_t dim_vec, uint8_t in_shift, q15_t * p_out);

/* Stopwatch - holds the runtime when accelerator finishes */
extern volatile uint32_t cnn_image_time;
extern volatile uint32_t cnn_voice_time;


void memcpy32_const(uint32_t *dst, int n);

/* Custom memcopy routines used for weights and data */
void memcpy32(uint32_t* dst, const uint32_t* src, int n);




/* Disable clocks and power to accelerator */
int cnn_disable(void);


/* Turn off the boost circuit */
int cnn_boost_disable(mxc_gpio_regs_t *port, uint32_t pin);

/* Turn on the boost circuit */
int cnn_boost_enable(mxc_gpio_regs_t *port, uint32_t pin);

/* Enable clocks and power to accelerator, enable interrupt */
int cnn_enable(uint32_t clock_source, uint32_t clock_divider);

/* Custom memcopy routines used for weights and data */
void memcpy32(uint32_t *dst, const uint32_t *src, int n);

/* Force stop accelerator */
int cnn_stop(void);

/* Continue accelerator after stop */
int cnn_continue(void);

void CNN_ISR(void);
#endif // __CNN_H__
