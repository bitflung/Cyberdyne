/**************************************************************************************************
* Copyright (C) 2020-2021 Maxim Integrated Products, Inc. All Rights Reserved.
*
* Maxim Integrated Products, Inc. Default Copyright Notice:
* https://www.maximintegrated.com/en/aboutus/legal/copyrights.html
**************************************************************************************************/

/*
 * This header file was automatically generated for the kws20_v3 network from a template.
 * Please do not edit; instead, edit the template and regenerate.
 */

#ifndef __CNN_VOICE_H__
#define __CNN_VOICE_H__

#include <stdint.h>
#include "weights_voice.h"
#include "cnn.h"


/*
  SUMMARY OF OPS
  Hardware: 8,402,528 ops (8,345,344 macc; 54,496 comp; 2,688 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 169,472 bytes out of 442,368 bytes total (38%)
  Bias memory:   0 bytes out of 2,048 bytes total (0%)
*/

/* Number of outputs for this network */
#define CNN_NUM_VOICE_OUTPUTS 21

/* Use this timer to time the inference */
#define CNN_INFERENCE_TIMER MXC_TMR0

/* Port pin actions used to signal that processing is active */
#define CNN_START LED_On(1)
#define CNN_COMPLETE LED_Off(1)
#define SYS_START LED_On(0)
#define SYS_COMPLETE LED_Off(0)

/* Stopwatch - holds the runtime when accelerator finishes */
extern volatile uint32_t cnn_voice_time;





/* Perform minimum accelerator initialization so it can be configured */
int cnn_init_voice(void);

/* Configure accelerator for the given network */
int cnn_configure_voice(void);

/* Load accelerator weights */
int cnn_load_weights_voice(void);

/* Verify accelerator weights (debug only) */
int cnn_verify_weights(void);

/* Load accelerator bias values (if needed) */
//int cnn_load_bias(void);

/* Start accelerator processing */
int cnn_start_voice(void);

/* Unload results from accelerator */
int cnn_unload_voice(uint32_t* out_buf);

/* Turn off the boost circuit */
int cnn_boost_disable_voice(mxc_gpio_regs_t* port, uint32_t pin);

#endif // __CNN_H__
