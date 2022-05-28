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

// kws20_v3
// Created using ./ai8xize.py --verbose --log --test-dir sdk/Examples/MAX78000/CNN --prefix kws20_v3 --checkpoint-file trained/ai85-kws20_v3-qat8-q.pth.tar --config-file networks/kws20-v3-hwc.yaml --softmax --device MAX78000 --compact-data --mexpress --timer 0 --display-checkpoint

// DO NOT EDIT - regenerate this file instead!

// Configuring 9 layers:
// Layer 0: 128x128 (HWC data), no pooling, conv1d with kernel size 1, stride 1, pad 0, 100x128 output
// Layer 1: 100x128 (HWC data), no pooling, conv1d with kernel size 3, stride 1, pad 0, 96x126 output
// Layer 2: 96x126 (HWC data), 2 max pool with stride 2, conv1d with kernel size 3, stride 1, pad 1, 64x63 output
// Layer 3: 64x63 (HWC data), no pooling, conv1d with kernel size 3, stride 1, pad 0, 48x61 output
// Layer 4: 48x61 (HWC data), 2 max pool with stride 2, conv1d with kernel size 3, stride 1, pad 1, 64x30 output
// Layer 5: 64x30 (HWC data), no pooling, conv1d with kernel size 3, stride 1, pad 0, 96x28 output
// Layer 6: 96x28 (HWC data), 2 avg pool with stride 2, conv1d with kernel size 3, stride 1, pad 1, 100x14 output
// Layer 7: 100x14 (HWC data), 2 max pool with stride 2, conv1d with kernel size 6, stride 1, pad 1, 64x4 output
// Layer 8: 64x4x1 (flattened HWC data), no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, 21x1x1 output

#include "cnn_voice.h"
#include "cnn.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc.h"
#include "gcfr_regs.h"

#include "weights_voice.h"



// Kernels:
static const uint32_t kernels_0[] = V_KERNELS_0;
static const uint32_t kernels_1[] = V_KERNELS_1;
static const uint32_t kernels_2[] = V_KERNELS_2;
static const uint32_t kernels_3[] = V_KERNELS_3;
static const uint32_t kernels_4[] = V_KERNELS_4;
static const uint32_t kernels_5[] = V_KERNELS_5;
static const uint32_t kernels_6[] = V_KERNELS_6;
static const uint32_t kernels_7[] = V_KERNELS_7;
static const uint32_t kernels_8[] = V_KERNELS_8;
static const uint32_t kernels_9[] = V_KERNELS_9;
static const uint32_t kernels_10[] = V_KERNELS_10;
static const uint32_t kernels_11[] = V_KERNELS_11;
static const uint32_t kernels_12[] = V_KERNELS_12;
static const uint32_t kernels_13[] = V_KERNELS_13;
static const uint32_t kernels_14[] = V_KERNELS_14;
static const uint32_t kernels_15[] = V_KERNELS_15;
static const uint32_t kernels_16[] = V_KERNELS_16;
static const uint32_t kernels_17[] = V_KERNELS_17;
static const uint32_t kernels_18[] = V_KERNELS_18;
static const uint32_t kernels_19[] = V_KERNELS_19;
static const uint32_t kernels_20[] = V_KERNELS_20;
static const uint32_t kernels_21[] = V_KERNELS_21;
static const uint32_t kernels_22[] = V_KERNELS_22;
static const uint32_t kernels_23[] = V_KERNELS_23;
static const uint32_t kernels_24[] = V_KERNELS_24;
static const uint32_t kernels_25[] = V_KERNELS_25;
static const uint32_t kernels_26[] = V_KERNELS_26;
static const uint32_t kernels_27[] = V_KERNELS_27;
static const uint32_t kernels_28[] = V_KERNELS_28;
static const uint32_t kernels_29[] = V_KERNELS_29;
static const uint32_t kernels_30[] = V_KERNELS_30;
static const uint32_t kernels_31[] = V_KERNELS_31;
static const uint32_t kernels_32[] = V_KERNELS_32;
static const uint32_t kernels_33[] = V_KERNELS_33;
static const uint32_t kernels_34[] = V_KERNELS_34;
static const uint32_t kernels_35[] = V_KERNELS_35;
static const uint32_t kernels_36[] = V_KERNELS_36;
static const uint32_t kernels_37[] = V_KERNELS_37;
static const uint32_t kernels_38[] = V_KERNELS_38;
static const uint32_t kernels_39[] = V_KERNELS_39;
static const uint32_t kernels_40[] = V_KERNELS_40;
static const uint32_t kernels_41[] = V_KERNELS_41;
static const uint32_t kernels_42[] = V_KERNELS_42;
static const uint32_t kernels_43[] = V_KERNELS_43;
static const uint32_t kernels_44[] = V_KERNELS_44;
static const uint32_t kernels_45[] = V_KERNELS_45;
static const uint32_t kernels_46[] = V_KERNELS_46;
static const uint32_t kernels_47[] = V_KERNELS_47;
static const uint32_t kernels_48[] = V_KERNELS_48;
static const uint32_t kernels_49[] = V_KERNELS_49;
static const uint32_t kernels_50[] = V_KERNELS_50;
static const uint32_t kernels_51[] = V_KERNELS_51;
static const uint32_t kernels_52[] = V_KERNELS_52;
static const uint32_t kernels_53[] = V_KERNELS_53;
static const uint32_t kernels_54[] = V_KERNELS_54;
static const uint32_t kernels_55[] = V_KERNELS_55;
static const uint32_t kernels_56[] = V_KERNELS_56;
static const uint32_t kernels_57[] = V_KERNELS_57;
static const uint32_t kernels_58[] = V_KERNELS_58;
static const uint32_t kernels_59[] = V_KERNELS_59;
static const uint32_t kernels_60[] = V_KERNELS_60;
static const uint32_t kernels_61[] = V_KERNELS_61;
static const uint32_t kernels_62[] = V_KERNELS_62;
static const uint32_t kernels_63[] = V_KERNELS_63;

int cnn_load_weights_voice(void)
{
	  *((volatile uint8_t *) 0x50180001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50180000, kernels_0, 1249);
	  *((volatile uint8_t *) 0x50184001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50184000, kernels_1, 1249);
	  *((volatile uint8_t *) 0x50188001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50188000, kernels_2, 1249);
	  *((volatile uint8_t *) 0x5018c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x5018c000, kernels_3, 1249);
	  *((volatile uint8_t *) 0x50190001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50190000, kernels_4, 1249);
	  *((volatile uint8_t *) 0x50194001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50194000, kernels_5, 1249);
	  *((volatile uint8_t *) 0x50198001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50198000, kernels_6, 1249);
	  *((volatile uint8_t *) 0x5019c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x5019c000, kernels_7, 576);
	  *((volatile uint8_t *) 0x501a0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501a0000, kernels_8, 576);
	  *((volatile uint8_t *) 0x501a4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501a4000, kernels_9, 576);
	  *((volatile uint8_t *) 0x501a8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501a8000, kernels_10, 576);
	  *((volatile uint8_t *) 0x501ac001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501ac000, kernels_11, 576);
	  *((volatile uint8_t *) 0x501b0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501b0000, kernels_12, 576);
	  *((volatile uint8_t *) 0x501b4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501b4000, kernels_13, 576);
	  *((volatile uint8_t *) 0x501b8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501b8000, kernels_14, 576);
	  *((volatile uint8_t *) 0x501bc001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x501bc000, kernels_15, 576);
	  *((volatile uint8_t *) 0x50580001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50580000, kernels_16, 576);
	  *((volatile uint8_t *) 0x50584001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50584000, kernels_17, 576);
	  *((volatile uint8_t *) 0x50588001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50588000, kernels_18, 576);
	  *((volatile uint8_t *) 0x5058c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x5058c000, kernels_19, 576);
	  *((volatile uint8_t *) 0x50590001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50590000, kernels_20, 576);
	  *((volatile uint8_t *) 0x50594001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50594000, kernels_21, 576);
	  *((volatile uint8_t *) 0x50598001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50598000, kernels_22, 576);
	  *((volatile uint8_t *) 0x5059c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x5059c000, kernels_23, 576);
	  *((volatile uint8_t *) 0x505a0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505a0000, kernels_24, 576);
	  *((volatile uint8_t *) 0x505a4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505a4000, kernels_25, 576);
	  *((volatile uint8_t *) 0x505a8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505a8000, kernels_26, 576);
	  *((volatile uint8_t *) 0x505ac001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505ac000, kernels_27, 576);
	  *((volatile uint8_t *) 0x505b0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505b0000, kernels_28, 576);
	  *((volatile uint8_t *) 0x505b4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505b4000, kernels_29, 576);
	  *((volatile uint8_t *) 0x505b8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505b8000, kernels_30, 576);
	  *((volatile uint8_t *) 0x505bc001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x505bc000, kernels_31, 576);
	  *((volatile uint8_t *) 0x50980001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50980000, kernels_32, 576);
	  *((volatile uint8_t *) 0x50984001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50984000, kernels_33, 576);
	  *((volatile uint8_t *) 0x50988001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50988000, kernels_34, 576);
	  *((volatile uint8_t *) 0x5098c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x5098c000, kernels_35, 576);
	  *((volatile uint8_t *) 0x50990001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50990000, kernels_36, 576);
	  *((volatile uint8_t *) 0x50994001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50994000, kernels_37, 576);
	  *((volatile uint8_t *) 0x50998001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50998000, kernels_38, 576);
	  *((volatile uint8_t *) 0x5099c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x5099c000, kernels_39, 576);
	  *((volatile uint8_t *) 0x509a0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509a0000, kernels_40, 576);
	  *((volatile uint8_t *) 0x509a4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509a4000, kernels_41, 576);
	  *((volatile uint8_t *) 0x509a8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509a8000, kernels_42, 576);
	  *((volatile uint8_t *) 0x509ac001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509ac000, kernels_43, 576);
	  *((volatile uint8_t *) 0x509b0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509b0000, kernels_44, 576);
	  *((volatile uint8_t *) 0x509b4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509b4000, kernels_45, 576);
	  *((volatile uint8_t *) 0x509b8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509b8000, kernels_46, 576);
	  *((volatile uint8_t *) 0x509bc001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x509bc000, kernels_47, 576);
	  *((volatile uint8_t *) 0x50d80001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d80000, kernels_48, 576);
	  *((volatile uint8_t *) 0x50d84001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d84000, kernels_49, 576);
	  *((volatile uint8_t *) 0x50d88001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d88000, kernels_50, 576);
	  *((volatile uint8_t *) 0x50d8c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d8c000, kernels_51, 576);
	  *((volatile uint8_t *) 0x50d90001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d90000, kernels_52, 576);
	  *((volatile uint8_t *) 0x50d94001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d94000, kernels_53, 576);
	  *((volatile uint8_t *) 0x50d98001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d98000, kernels_54, 576);
	  *((volatile uint8_t *) 0x50d9c001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50d9c000, kernels_55, 576);
	  *((volatile uint8_t *) 0x50da0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50da0000, kernels_56, 576);
	  *((volatile uint8_t *) 0x50da4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50da4000, kernels_57, 576);
	  *((volatile uint8_t *) 0x50da8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50da8000, kernels_58, 576);
	  *((volatile uint8_t *) 0x50dac001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50dac000, kernels_59, 576);
	  *((volatile uint8_t *) 0x50db0001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50db0000, kernels_60, 576);
	  *((volatile uint8_t *) 0x50db4001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50db4000, kernels_61, 576);
	  *((volatile uint8_t *) 0x50db8001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50db8000, kernels_62, 576);
	  *((volatile uint8_t *) 0x50dbc001) = 0x01; // Set address
	  memcpy32((uint32_t *) 0x50dbc000, kernels_63, 576);

	  return CNN_OK;
}

int cnn_load_bias(void)
{
    // Not used in this network
    return CNN_OK;
}

int cnn_init_voice(void)
{
	*((volatile uint32_t*) 0x50001000) = 0x00000000; // AON control
	*((volatile uint32_t*) 0x50100000) = 0x00100008; // Stop SM
	*((volatile uint32_t*) 0x50100004) = 0x0000040e; // SRAM control
	*((volatile uint32_t*) 0x50100008) = 0x00000009; // Layer count
	*((volatile uint32_t*) 0x50500000) = 0x00100008; // Stop SM
	*((volatile uint32_t*) 0x50500004) = 0x0000040e; // SRAM control
	*((volatile uint32_t*) 0x50500008) = 0x00000009; // Layer count
	*((volatile uint32_t*) 0x50900000) = 0x00100008; // Stop SM
	*((volatile uint32_t*) 0x50900004) = 0x0000040e; // SRAM control
	*((volatile uint32_t*) 0x50900008) = 0x00000009; // Layer count
	*((volatile uint32_t*) 0x50d00000) = 0x00100008; // Stop SM
	*((volatile uint32_t*) 0x50d00004) = 0x0000040e; // SRAM control
	*((volatile uint32_t*) 0x50d00008) = 0x00000009; // Layer count

	return CNN_OK;
}

int cnn_configure_voice(void)
{
	// Layer 0 quadrant 0
	*((volatile uint32_t*) 0x50100010) = 0x0000007f; // Rows
	*((volatile uint32_t*) 0x50100310) = 0x00000800; // SRAM write ptr
	*((volatile uint32_t*) 0x50100410) = 0x00002000; // Write ptr mask offs
	*((volatile uint32_t*) 0x50100490) = 0x00000001; // Write ptr multi-pass channel offs
	*((volatile uint32_t*) 0x50100590) = 0x0000eb20; // Layer control
	*((volatile uint32_t*) 0x50100a10) = 0x00019811; // Layer control 2
	*((volatile uint32_t*) 0x50100610) = 0x00000678; // Mask offset and count
	*((volatile uint32_t*) 0x50100110) = 0x00001100; // 1D
	*((volatile uint32_t*) 0x50100790) = 0x00002000; // Post processing register
	*((volatile uint32_t*) 0x50100710) = 0xffffffff; // Mask and processor enables

	  // Layer 0 quadrant 1
	  *((volatile uint32_t *) 0x50500010) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50500310) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50500410) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50500490) = 0x00000001; // Write ptr multi-pass channel offs
	  *((volatile uint32_t *) 0x50500590) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a10) = 0x00019811; // Layer control 2
	  *((volatile uint32_t *) 0x50500610) = 0x00000678; // Mask offset and count
	  *((volatile uint32_t *) 0x50500110) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50500790) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50500710) = 0xffffffff; // Mask and processor enables

	  // Layer 0 quadrant 2
	  *((volatile uint32_t *) 0x50900010) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50900310) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50900410) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50900490) = 0x00000001; // Write ptr multi-pass channel offs
	  *((volatile uint32_t *) 0x50900590) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a10) = 0x00019811; // Layer control 2
	  *((volatile uint32_t *) 0x50900610) = 0x00000678; // Mask offset and count
	  *((volatile uint32_t *) 0x50900110) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50900790) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50900710) = 0xffffffff; // Mask and processor enables

	  // Layer 0 quadrant 3
	  *((volatile uint32_t *) 0x50d00010) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50d00310) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50d00410) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50d00490) = 0x00000001; // Write ptr multi-pass channel offs
	  *((volatile uint32_t *) 0x50d00590) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a10) = 0x00019811; // Layer control 2
	  *((volatile uint32_t *) 0x50d00610) = 0x00000678; // Mask offset and count
	  *((volatile uint32_t *) 0x50d00110) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50d00790) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50d00710) = 0xffffffff; // Mask and processor enables

	  // Layer 1 quadrant 0
	  *((volatile uint32_t *) 0x50100014) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50100414) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50100494) = 0x00000001; // Write ptr multi-pass channel offs
	  *((volatile uint32_t *) 0x50100514) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x50100594) = 0x0000eb20; // Layer control
	  *((volatile uint32_t *) 0x50100a14) = 0x00019811; // Layer control 2
	  *((volatile uint32_t *) 0x50100614) = 0x06c00d38; // Mask offset and count
	  *((volatile uint32_t *) 0x50100114) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50100794) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50100714) = 0xffffffff; // Mask and processor enables

	  // Layer 1 quadrant 1
	  *((volatile uint32_t *) 0x50500014) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50500414) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50500494) = 0x00000001; // Write ptr multi-pass channel offs
	  *((volatile uint32_t *) 0x50500514) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x50500594) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a14) = 0x00019811; // Layer control 2
	  *((volatile uint32_t *) 0x50500614) = 0x06c00d38; // Mask offset and count
	  *((volatile uint32_t *) 0x50500114) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50500794) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50500714) = 0xffffffff; // Mask and processor enables

	  // Layer 1 quadrant 2
	  *((volatile uint32_t *) 0x50900014) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50900414) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50900494) = 0x00000001; // Write ptr multi-pass channel offs
	  *((volatile uint32_t *) 0x50900514) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x50900594) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a14) = 0x00019811; // Layer control 2
	  *((volatile uint32_t *) 0x50900614) = 0x06c00d38; // Mask offset and count
	  *((volatile uint32_t *) 0x50900114) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50900794) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50900714) = 0xffffffff; // Mask and processor enables

	  // Layer 1 quadrant 3
	  *((volatile uint32_t *) 0x50d00014) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50d00414) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50d00494) = 0x00000001; // Write ptr multi-pass channel offs
	  *((volatile uint32_t *) 0x50d00514) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x50d00594) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a14) = 0x00019811; // Layer control 2
	  *((volatile uint32_t *) 0x50d00614) = 0x06c00d38; // Mask offset and count
	  *((volatile uint32_t *) 0x50d00114) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50d00794) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50d00714) = 0x000f000f; // Mask and processor enables

	  // Layer 2 quadrant 0
	  *((volatile uint32_t *) 0x50100018) = 0x0000007f; // Rows


	  *((volatile uint32_t *) 0x50100318) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50100418) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50100598) = 0x0000eb20; // Layer control
	  *((volatile uint32_t *) 0x50100a18) = 0x00018801; // Layer control 2
	  *((volatile uint32_t *) 0x50100618) = 0x0d801098; // Mask offset and count
	  *((volatile uint32_t *) 0x50100118) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50100798) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50100718) = 0xffffffff; // Mask and processor enables

	  // Layer 2 quadrant 1
	  *((volatile uint32_t *) 0x50500018) = 0x0000007f; // Rows


	  *((volatile uint32_t *) 0x50500318) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50500418) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50500598) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a18) = 0x00018801; // Layer control 2
	  *((volatile uint32_t *) 0x50500618) = 0x0d801098; // Mask offset and count
	  *((volatile uint32_t *) 0x50500118) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50500798) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50500718) = 0xffffffff; // Mask and processor enables

	  // Layer 2 quadrant 2
	  *((volatile uint32_t *) 0x50900018) = 0x0000007f; // Rows


	  *((volatile uint32_t *) 0x50900318) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50900418) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50900598) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a18) = 0x00018801; // Layer control 2
	  *((volatile uint32_t *) 0x50900618) = 0x0d801098; // Mask offset and count
	  *((volatile uint32_t *) 0x50900118) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50900798) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50900718) = 0xffffffff; // Mask and processor enables

	  // Layer 2 quadrant 3
	  *((volatile uint32_t *) 0x50d00018) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50d00318) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50d00418) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50d00598) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a18) = 0x00018801; // Layer control 2
	  *((volatile uint32_t *) 0x50d00618) = 0x0d801098; // Mask offset and count
	  *((volatile uint32_t *) 0x50d00118) = 0x00001100; // 1D
	  *((volatile uint32_t *) 0x50d00798) = 0x00002000; // Post processing register
	  *((volatile uint32_t *) 0x50d00718) = 0x000f000f; // Mask and processor enables


	  // Layer 3 quadrant 0
	  *((volatile uint32_t *) 0x5010001c) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x5010041c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x5010051c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x5010059c) = 0x0000eb20; // Layer control
	  *((volatile uint32_t *) 0x50100a1c) = 0x00007800; // Layer control 2
	  *((volatile uint32_t *) 0x5010061c) = 0x10e01158; // Mask offset and count
	  *((volatile uint32_t *) 0x5010011c) = 0x00001100; // 1D

	  *((volatile uint32_t *) 0x5010071c) = 0xffffffff; // Mask and processor enables

	  // Layer 3 quadrant 1
	  *((volatile uint32_t *) 0x5050001c) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x5050041c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x5050051c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x5050059c) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a1c) = 0x00007800; // Layer control 2
	  *((volatile uint32_t *) 0x5050061c) = 0x10e01158; // Mask offset and count
	  *((volatile uint32_t *) 0x5050011c) = 0x00001100; // 1D

	  *((volatile uint32_t *) 0x5050071c) = 0xffffffff; // Mask and processor enables

	  // Layer 3 quadrant 2
	  *((volatile uint32_t *) 0x5090001c) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x5090041c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x5090051c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x5090059c) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a1c) = 0x00007800; // Layer control 2
	  *((volatile uint32_t *) 0x5090061c) = 0x10e01158; // Mask offset and count
	  *((volatile uint32_t *) 0x5090011c) = 0x00001100; // 1D

	  *((volatile uint32_t *) 0x5090071c) = 0xffffffff; // Mask and processor enables

	  // Layer 3 quadrant 3
	  *((volatile uint32_t *) 0x50d0001c) = 0x0000007f; // Rows
	  *((volatile uint32_t *) 0x50d0041c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x50d0051c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x50d0059c) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a1c) = 0x00007800; // Layer control 2
	  *((volatile uint32_t *) 0x50d0061c) = 0x10e01158; // Mask offset and count
	  *((volatile uint32_t *) 0x50d0011c) = 0x00001100; // 1D

	  *((volatile uint32_t *) 0x50d0071c) = 0x00030003; // Mask and processor enables

	  // Layer 4 quadrant 0
	  *((volatile uint32_t *) 0x50100020) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x501000a0) = 0x00010009; // Columns

	  *((volatile uint32_t *) 0x50100320) = 0x00002800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50100420) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x501005a0) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50100a20) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x50100620) = 0x020002f8; // Mask offset and count
	  *((volatile uint32_t *) 0x501006a0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x501007a0) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50100720) = 0xffffffff; // Mask and processor enables

	  // Layer 4 quadrant 1
	  *((volatile uint32_t *) 0x50500020) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x505000a0) = 0x00010009; // Columns

	  *((volatile uint32_t *) 0x50500320) = 0x00002800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50500420) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x505005a0) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a20) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x50500620) = 0x020002f8; // Mask offset and count
	  *((volatile uint32_t *) 0x505006a0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x505007a0) = 0x00004000; // Post processing register


	  // Layer 4 quadrant 2
	  *((volatile uint32_t *) 0x50900020) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x509000a0) = 0x00010009; // Columns

	  *((volatile uint32_t *) 0x50900320) = 0x00002800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50900420) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x509005a0) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a20) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x50900620) = 0x020002f8; // Mask offset and count
	  *((volatile uint32_t *) 0x509006a0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x509007a0) = 0x00004000; // Post processing register


	  // Layer 4 quadrant 3
	  *((volatile uint32_t *) 0x50d00020) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x50d000a0) = 0x00010009; // Columns

	  *((volatile uint32_t *) 0x50d00320) = 0x00002800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50d00420) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x50d005a0) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a20) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x50d00620) = 0x020002f8; // Mask offset and count
	  *((volatile uint32_t *) 0x50d006a0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x50d007a0) = 0x00004000; // Post processing register

	  // Layer 5 quadrant 0
	  *((volatile uint32_t *) 0x50100024) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x501000a4) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50100424) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50100524) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x501005a4) = 0x00006b20; // Layer control
	  *((volatile uint32_t *) 0x50100a24) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50100624) = 0x030004f8; // Mask offset and count
	  *((volatile uint32_t *) 0x501006a4) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x501007a4) = 0x00006000; // Post processing register
	  *((volatile uint32_t *) 0x50100724) = 0xfff0fff0; // Mask and processor enables

	  // Layer 5 quadrant 1
	  *((volatile uint32_t *) 0x50500024) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x505000a4) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50500424) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50500524) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x505005a4) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a24) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50500624) = 0x030004f8; // Mask offset and count
	  *((volatile uint32_t *) 0x505006a4) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x505007a4) = 0x00006000; // Post processing register
	  *((volatile uint32_t *) 0x50500724) = 0xffffffff; // Mask and processor enables

	  // Layer 5 quadrant 2
	  *((volatile uint32_t *) 0x50900024) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x509000a4) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50900424) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50900524) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x509005a4) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a24) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50900624) = 0x030004f8; // Mask offset and count
	  *((volatile uint32_t *) 0x509006a4) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x509007a4) = 0x00006000; // Post processing register
	  *((volatile uint32_t *) 0x50900724) = 0x000f000f; // Mask and processor enables

	  // Layer 5 quadrant 3
	  *((volatile uint32_t *) 0x50d00024) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x50d000a4) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50d00424) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50d00524) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x50d005a4) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a24) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50d00624) = 0x030004f8; // Mask offset and count
	  *((volatile uint32_t *) 0x50d006a4) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x50d007a4) = 0x00006000; // Post processing register


	  // Layer 6 quadrant 0
	  *((volatile uint32_t *) 0x50100028) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x501000a8) = 0x00010009; // Columns

	  *((volatile uint32_t *) 0x50100328) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50100428) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x501005a8) = 0x0000eb20; // Layer control
	  *((volatile uint32_t *) 0x50100a28) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50100628) = 0x050006f8; // Mask offset and count
	  *((volatile uint32_t *) 0x501006a8) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x501007a8) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50100728) = 0xffffffff; // Mask and processor enables

	  // Layer 6 quadrant 1
	  *((volatile uint32_t *) 0x50500028) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x505000a8) = 0x00010009; // Columns

	  *((volatile uint32_t *) 0x50500328) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50500428) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x505005a8) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a28) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50500628) = 0x050006f8; // Mask offset and count
	  *((volatile uint32_t *) 0x505006a8) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x505007a8) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50500728) = 0xffffffff; // Mask and processor enables

	  // Layer 6 quadrant 2
	  *((volatile uint32_t *) 0x50900028) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x509000a8) = 0x00010009; // Columns

	  *((volatile uint32_t *) 0x50900328) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50900428) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x509005a8) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a28) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50900628) = 0x050006f8; // Mask offset and count
	  *((volatile uint32_t *) 0x509006a8) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x509007a8) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50900728) = 0xffffffff; // Mask and processor enables

	  // Layer 6 quadrant 3
	  *((volatile uint32_t *) 0x50d00028) = 0x00010011; // Rows

	  *((volatile uint32_t *) 0x50d000a8) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50d00328) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50d00428) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50d005a8) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a28) = 0x0001f800; // Layer control 2
	  *((volatile uint32_t *) 0x50d00628) = 0x050006f8; // Mask offset and count
	  *((volatile uint32_t *) 0x50d006a8) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x50d007a8) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50d00728) = 0xffffffff; // Mask and processor enables

	  // Layer 7 quadrant 0
	  *((volatile uint32_t *) 0x5010002c) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x501000ac) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x5010032c) = 0x00010000; // SRAM write ptr
	  *((volatile uint32_t *) 0x5010042c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x5010052c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x501005ac) = 0x0000eb20; // Layer control
	  *((volatile uint32_t *) 0x50100a2c) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x5010062c) = 0x070007f8; // Mask offset and count
	  *((volatile uint32_t *) 0x501006ac) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x501007ac) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x5010072c) = 0xffffffff; // Mask and processor enables

	  // Layer 7 quadrant 1
	  *((volatile uint32_t *) 0x5050002c) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x505000ac) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x5050032c) = 0x00010000; // SRAM write ptr
	  *((volatile uint32_t *) 0x5050042c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x5050052c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x505005ac) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a2c) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x5050062c) = 0x070007f8; // Mask offset and count
	  *((volatile uint32_t *) 0x505006ac) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x505007ac) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x5050072c) = 0xffffffff; // Mask and processor enables

	  // Layer 7 quadrant 2
	  *((volatile uint32_t *) 0x5090002c) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x509000ac) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x5090032c) = 0x00010000; // SRAM write ptr
	  *((volatile uint32_t *) 0x5090042c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x5090052c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x509005ac) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a2c) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x5090062c) = 0x070007f8; // Mask offset and count
	  *((volatile uint32_t *) 0x509006ac) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x509007ac) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x5090072c) = 0xffffffff; // Mask and processor enables

	  // Layer 7 quadrant 3
	  *((volatile uint32_t *) 0x50d0002c) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x50d000ac) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50d0032c) = 0x00010000; // SRAM write ptr
	  *((volatile uint32_t *) 0x50d0042c) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x50d0052c) = 0x00000800; // SRAM read ptr
	  *((volatile uint32_t *) 0x50d005ac) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a2c) = 0x0000f800; // Layer control 2
	  *((volatile uint32_t *) 0x50d0062c) = 0x070007f8; // Mask offset and count
	  *((volatile uint32_t *) 0x50d006ac) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x50d007ac) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50d0072c) = 0xffffffff; // Mask and processor enables

	  // Layer 8 quadrant 0
	  *((volatile uint32_t *) 0x50100030) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x501000b0) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50100430) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x501005b0) = 0x0000cb20; // Layer control
	  *((volatile uint32_t *) 0x50100a30) = 0x00003000; // Layer control 2
	  *((volatile uint32_t *) 0x50100630) = 0x02000230; // Mask offset and count
	  *((volatile uint32_t *) 0x501006b0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x501007b0) = 0x00004000; // Post processing register

	  // Layer 8 quadrant 1
	  *((volatile uint32_t *) 0x50500030) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x505000b0) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50500430) = 0x00002000; // Write ptr mask offs

	  *((volatile uint32_t *) 0x505005b0) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50500a30) = 0x00003000; // Layer control 2
	  *((volatile uint32_t *) 0x50500630) = 0x02000230; // Mask offset and count
	  *((volatile uint32_t *) 0x505006b0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x505007b0) = 0x00004000; // Post processing register

	  // Layer 8 quadrant 2
	  *((volatile uint32_t *) 0x50900030) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x509000b0) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50900430) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x509005b0) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50900a30) = 0x00003000; // Layer control 2
	  *((volatile uint32_t *) 0x50900630) = 0x02000230; // Mask offset and count
	  *((volatile uint32_t *) 0x509006b0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x509007b0) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50900730) = 0xfffcfffc; // Mask and processor enables

	  // Layer 8 quadrant 3
	  *((volatile uint32_t *) 0x50d00030) = 0x00010011; // Rows
	  *((volatile uint32_t *) 0x50d000b0) = 0x00010009; // Columns
	  *((volatile uint32_t *) 0x50d00430) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50d005b0) = 0x00000b20; // Layer control
	  *((volatile uint32_t *) 0x50d00a30) = 0x00003000; // Layer control 2
	  *((volatile uint32_t *) 0x50d00630) = 0x02000230; // Mask offset and count
	  *((volatile uint32_t *) 0x50d006b0) = 0x00000007; // TRAM ptr max
	  *((volatile uint32_t *) 0x50d007b0) = 0x00004000; // Post processing register
	  *((volatile uint32_t *) 0x50d00730) = 0xffffffff; // Mask and processor enables

	  // Layer 9 quadrant 0
	  *((volatile uint32_t *) 0x50100334) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x501003b4) = 0x00000001; // Write ptr time slot offs
	  *((volatile uint32_t *) 0x50100434) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x501005b4) = 0x00010920; // Layer control
	  *((volatile uint32_t *) 0x50100a34) = 0x0000a00f; // Layer control 2
	  *((volatile uint32_t *) 0x50100634) = 0x48009bf8; // Mask offset and count
	  *((volatile uint32_t *) 0x50100134) = 0x00000100; // 1D
	  *((volatile uint32_t *) 0x501006b4) = 0x007f00ff; // TRAM ptr max
	  *((volatile uint32_t *) 0x501007b4) = 0x081ca000; // Post processing register
	  *((volatile uint32_t *) 0x50100734) = 0x007f007f; // Mask and processor enables

	  // Layer 9 quadrant 1
	  *((volatile uint32_t *) 0x50500334) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x505003b4) = 0x00000001; // Write ptr time slot offs
	  *((volatile uint32_t *) 0x50500434) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x505005b4) = 0x00010920; // Layer control
	  *((volatile uint32_t *) 0x50500a34) = 0x0000a00f; // Layer control 2
	  *((volatile uint32_t *) 0x50500634) = 0x48009bf8; // Mask offset and count
	  *((volatile uint32_t *) 0x50500134) = 0x00000100; // 1D
	  *((volatile uint32_t *) 0x505006b4) = 0x007f00ff; // TRAM ptr max
	  *((volatile uint32_t *) 0x505007b4) = 0x081ca000; // Post processing register

	  // Layer 9 quadrant 2
	  *((volatile uint32_t *) 0x50900334) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x509003b4) = 0x00000001; // Write ptr time slot offs
	  *((volatile uint32_t *) 0x50900434) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x509005b4) = 0x00010920; // Layer control
	  *((volatile uint32_t *) 0x50900a34) = 0x0000a00f; // Layer control 2
	  *((volatile uint32_t *) 0x50900634) = 0x48009bf8; // Mask offset and count
	  *((volatile uint32_t *) 0x50900134) = 0x00000100; // 1D
	  *((volatile uint32_t *) 0x509006b4) = 0x007f00ff; // TRAM ptr max
	  *((volatile uint32_t *) 0x509007b4) = 0x081ca000; // Post processing register

	  // Layer 9 quadrant 3
	  *((volatile uint32_t *) 0x50d00334) = 0x00000800; // SRAM write ptr
	  *((volatile uint32_t *) 0x50d003b4) = 0x00000001; // Write ptr time slot offs
	  *((volatile uint32_t *) 0x50d00434) = 0x00002000; // Write ptr mask offs
	  *((volatile uint32_t *) 0x50d005b4) = 0x00010920; // Layer control
	  *((volatile uint32_t *) 0x50d00a34) = 0x0000a00f; // Layer control 2
	  *((volatile uint32_t *) 0x50d00634) = 0x48009bf8; // Mask offset and count
	  *((volatile uint32_t *) 0x50d00134) = 0x00000100; // 1D
	  *((volatile uint32_t *) 0x50d006b4) = 0x007f00ff; // TRAM ptr max
	  *((volatile uint32_t *) 0x50d007b4) = 0x081ca000; // Post processing register


	  return CNN_OK;
}

int cnn_start_voice(void)
{
	cnn_voice_time = 0;

    *((volatile uint32_t*) 0x50100000) = 0x00100808;  // Enable group 0
    *((volatile uint32_t*) 0x50500000) = 0x00100809;  // Enable group 1
    *((volatile uint32_t*) 0x50900000) = 0x00100809;  // Enable group 2
    *((volatile uint32_t*) 0x50d00000) = 0x00100809;  // Enable group 3

#ifdef CNN_INFERENCE_TIMER
    MXC_TMR_SW_Start(CNN_INFERENCE_TIMER);
#endif

    //CNN_START; // Allow capture of processing time
    *((volatile uint32_t*) 0x50100000) = 0x00100009;  // Master enable group 0

    return CNN_OK;
}

// Custom unload for this network: 32-bit data, shape: [21, 1, 1]
int cnn_unload_voice(uint32_t* out_buf)
{
    volatile uint32_t* addr;
    addr = (volatile uint32_t*) 0x50402000;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    addr = (volatile uint32_t*) 0x5040a000;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    addr = (volatile uint32_t*) 0x50412000;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    addr = (volatile uint32_t*) 0x5041a000;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    addr = (volatile uint32_t*) 0x50802000;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    *out_buf++ = *addr++;
    addr = (volatile uint32_t*) 0x5080a000;
    *out_buf++ = *addr++;

    return CNN_OK;
}


int cnn_boost_disable_voice(mxc_gpio_regs_t* port, uint32_t pin)
{
	  mxc_gpio_cfg_t gpio_out;
	  gpio_out.port = port;
	  gpio_out.mask = pin;
	  gpio_out.pad = MXC_GPIO_PAD_NONE;
	  gpio_out.func = MXC_GPIO_FUNC_OUT;
	  MXC_GPIO_Config(&gpio_out);
	  MXC_GPIO_OutClr(gpio_out.port, gpio_out.mask);

	  return CNN_OK;

}


