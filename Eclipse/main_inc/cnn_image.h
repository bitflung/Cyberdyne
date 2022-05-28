/*
 * cnn_image.h
 *
 *  Created on: May 26, 2022
 *      Author: jsjai
 */

#ifndef CNN_IMAGE_H_
#define CNN_IMAGE_H_

#include <stdint.h>
#include "weights_image.h"
#include "cnn.h"

/*
  SUMMARY OF OPS
  Hardware: 51,368,960 ops (50,432,000 macc; 936,960 comp; 0 add; 0 mul; 0 bitwise)
    Layer 0: 7,340,032 ops (7,077,888 macc; 262,144 comp; 0 add; 0 mul; 0 bitwise)
    Layer 1: 19,267,584 ops (18,874,368 macc; 393,216 comp; 0 add; 0 mul; 0 bitwise)
    Layer 2: 19,070,976 ops (18,874,368 macc; 196,608 comp; 0 add; 0 mul; 0 bitwise)
    Layer 3: 4,792,320 ops (4,718,592 macc; 73,728 comp; 0 add; 0 mul; 0 bitwise)
    Layer 4: 600,064 ops (589,824 macc; 10,240 comp; 0 add; 0 mul; 0 bitwise)
    Layer 5: 295,936 ops (294,912 macc; 1,024 comp; 0 add; 0 mul; 0 bitwise)
    Layer 6: 2,048 ops (2,048 macc; 0 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 57,776 bytes out of 442,368 bytes total (13%)
  Bias memory:   2 bytes out of 2,048 bytes total (0%)
*/

/* Number of outputs for this network */
#define CNN_NUM_IMAGE_OUTPUTS 2

extern volatile uint32_t cnn_voice_time;

/* Perform minimum accelerator initialization so it can be configured */
int cnn_init_image(void);

/* Configure accelerator for the given network */
int cnn_configure_image(void);

/* Load accelerator weights */
int cnn_load_weights_image(void);

/* Verify accelerator weights (debug only) */
int cnn_verify_weights(void);

/* Load accelerator bias values (if needed) */
int cnn_load_bias_image(void);

/* Start accelerator processing */
int cnn_image_start(void);

/* Unload results from accelerator */
int cnn_image_unload(uint32_t *out_buf);


#endif /* CNN_IMAGE_H_ */
